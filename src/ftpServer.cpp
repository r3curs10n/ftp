#include "../includes/ftpServer.h"
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

//testing stuff

namespace sys {
	string pwd(){return "troll";}
	string ls(string arg){return "troll ls";}
	bool cwd(string arg){return true;};
	string syst(){return "linux xx";}
};

int main(){
	ftpServer serv = ftpServer(4448);
	serv.start();
	return 0;
}

//end testing stuff

ftpServer::ftpServer(int port):
	m_port(port)
{}

bool ftpServer::start()
{
	if (!(m_control_sock.bind(m_port) && m_control_sock.listen())) return false;
	
	while (true)
	{
		tcpSocket client_sock = m_control_sock.accept();
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

// Processes request and returns false if no further requests from client are expeced (QUIT command)
bool ftpServer::processRequest(ftpRequest& req, tcpSocket& client_control_sock)
{
	if (req.getCmd() == "PWD")
	{
		client_control_sock.sendString( ftpResponse(257, sys::pwd()).toString() );
	}
	else if (req.getCmd() == "CWD")
	{
		if (sys::cwd(req.getArg()))
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
		client_control_sock.sendString( ftpResponse(150, "Here comes the file.").toString() );
		ifstream f(req.getArg().c_str());
		if (f.is_open())
		{
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
			//respond with some error
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
			client_control_sock.sendString( ftpResponse(226, "Transfer Complete.").toString() );
		}
		else
		{
			//respond with some error
		}
	}
	else if (req.getCmd() == "USER")
	{
		//TODO: actually implement login
		client_control_sock.sendString(ftpResponse(331, "Please specify password.").toString());
		string sreq;
		sreq = client_control_sock.recvString();
		client_control_sock.sendString(ftpResponse(230, "Login successful.").toString());
		
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
		string arg = req.getArg();
		cout<<arg<<endl;
		int i;
		for (i=0; i<arg.length() && arg[i]!=':'; i++);
		string hostname = arg.substr(0, i);
		string port = arg.substr(i+1, arg.length() - (i+1));
		stringstream s(port);
		unsigned short uport;
		s>>uport;
		cout<<hostname<<"  "<<uport<<endl;
		if (m_data_sock.connect(hostname, uport))
		{
			client_control_sock.sendString( ftpResponse(200, "PORT successful.").toString() );
		}
		else
		{
			//throw weired error
			client_control_sock.sendString( ftpResponse(000, "ERROR").toString() );
		}
	}
	else if (req.getCmd() == "QUIT")
	{
		client_control_sock.sendString( ftpResponse(221, "Goodbye.").toString() );
		return false;
	}
	
	return true;
}
