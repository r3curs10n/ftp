#include "../includes/ftpServer.h"
#include "../includes/sys.h"
#include <cstring>
/*

How the server works:

Main process listens on port 21 for incoming connections.
When a connection is received, a new process is forked to handle that client,
and this process exits when it has finished serving the client.

Sockets:
m_control_sock: This is the socket used to listen for incoming connections
				Its value is same for main process and forked processes.

m_data_sock: Set up when a client sends a PORT request.
			 Each forked process will have its own data socket (values will be different).

client_control_sock: This is passed to the function serveClient. It refers to
					 the connection between port 21 of server and control port of a specific client.

*/

ftpServer::ftpServer(int port):
	m_port(port)
{}

bool ftpServer::start()
{
	if (!(m_control_sock.bind(m_port) && m_control_sock.listen())) return false;
	
	while (true)
	{
		tcpSocket client_sock = m_control_sock.accept();
		
		cout << "Got connection from " << client_sock.getDestHostname() << ":" << client_sock.getDestPort() << endl;
		
		if (!fork())
		{
			//child
			serveClient(client_sock);
			exit(0);
		}
	}
}

void ftpServer::serveClient(tcpSocket client_control_sock)
{
	string sreq;
	ftpRequest req;
	
	//connection ack
	client_control_sock.sendString( ftpResponse(220, "machauServer").toString() );
	
	while ((sreq = client_control_sock.recvString()).length() > 0)
	{
		ftpRequest req = ftpRequest::parseFtpRequest(sreq);
		if (!processRequest( req, client_control_sock)) break;
	}
	
}

// Processes request and returns false if no further requests from client are expected (QUIT command)
bool ftpServer::processRequest(ftpRequest& req, tcpSocket& client_control_sock)
{
	if (req.getCmd() == "USER")
	{
		client_control_sock.sendString(ftpResponse(331, "Please specify password.").toString());
		ftpRequest passwd = ftpRequest::parseFtpRequest(client_control_sock.recvString());
		
		if(Auth.login(req.getArg(),passwd.getArg()))
		{
			client_control_sock.sendString(ftpResponse(230, "Login successful.").toString());
			sys::cd(Auth.getCurUser().directory);
			//cout << sys::setRootDir(Auth.getCurUser().directory) << endl;
			//sys::cd("/");
		}
		else
		{
			client_control_sock.sendString(ftpResponse(530, "Login incorrect.").toString());
		}
	}
	else if(!Auth.isLoggedin())
	{
		client_control_sock.sendString(ftpResponse(530, "Please login with USER and PASS.").toString());
	}
	else if (req.getCmd() == "PWD")
	{
		client_control_sock.sendString( ftpResponse(257, sys::pwd()).toString() );
	}
	else if (req.getCmd() == "CWD")
	{
		if (sys::cd(req.getArg()))
		{
			client_control_sock.sendString( ftpResponse(250, "Directory successfully changed.").toString() );
		}
		else
		{
			client_control_sock.sendString( ftpResponse(550, "Failed to change directory.").toString() );
		}
	}
	else if (req.getCmd() == "LIST")
	{
		client_control_sock.sendString( ftpResponse(150, "Here comes directory listing.").toString() );
		string result = sys::ls(req.getArg());
		m_data_sock.sendString(result);
		m_data_sock.close();
		client_control_sock.sendString( ftpResponse(226, "Directory send Ok.").toString() );
	}
	else if (req.getCmd() == "RETR")
	{
		ifstream f(req.getArg().c_str());
		if (f.is_open())
		{
			client_control_sock.sendString( ftpResponse(150, "Here comes the file.").toString() );
			char buffer[1024];
			
			while (!f.eof())
			{
				f.read(buffer, 1024);
				m_data_sock.sendData(buffer, f.gcount());
			}
			f.close();
			m_data_sock.close();
			client_control_sock.sendString( ftpResponse(226, "Transfer complete.").toString() );
		}
		else
		{
			m_data_sock.close();
			client_control_sock.sendString( ftpResponse(550, "File not present.").toString() );
		}
	}
	else if (req.getCmd() == "STOR")
	{
		ofstream f(req.getArg().c_str());
		if (f.is_open())
		{
			client_control_sock.sendString( ftpResponse(150, "Ok to send data.").toString() );
			char buffer[1024];
			int n;
			while ((n=m_data_sock.recvData(buffer, 1024))>0)
			{
				f.write(buffer, n);
			}
			f.close();
			m_data_sock.close();
			client_control_sock.sendString( ftpResponse(226, "Transfer Complete.").toString() );
		}
		else
		{
			m_data_sock.close();
			client_control_sock.sendString( ftpResponse(550, "Failed to create file.").toString() );
		}
	}
	else if (req.getCmd() == "SYST")
	{
		client_control_sock.sendString(ftpResponse(215, sys::syst()).toString());
	}
	else if (req.getCmd() == "TYPE")
	{
		client_control_sock.sendString(ftpResponse(200, "Mode switched.").toString());
	}
	else if (req.getCmd() == "PORT")
	{
		bool success = true;
		string hostname;
		unsigned short port;
		string sarg = req.getArg();
		char* arg = new char[sarg.length()];
		strcpy(arg, sarg.c_str());

		char* tok = strtok(arg, ":");
		if (tok) hostname = string(tok); else success = false;

		tok = strtok(NULL, ":");
		if (tok) port = atoi(tok); else success = false;

		if (success && m_data_sock.connect(hostname, port))
		{
			client_control_sock.sendString( ftpResponse(200, "PORT successful.").toString() );
		}
		else
		{
			client_control_sock.sendString( ftpResponse(500, "Illegal PORT command.").toString() );
		}

	}
	else if (req.getCmd() == "QUIT")
	{
		client_control_sock.sendString( ftpResponse(221, "Goodbye.").toString() );
		return false;
	}
	
	return true;
}
