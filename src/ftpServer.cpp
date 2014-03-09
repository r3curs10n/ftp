#include "../includes/ftpServer"

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
	string ls(){return "troll ls";}
	bool cwd(){return true;};
};

int main(){
	auto serv = ftpServer(4444);
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
	
	while ((sreq = client_control_sock.recvString()).length() > 0)
	{
		if (!processRequest( ftpRequest::parseFtpRequest(sreq), client_control_sock)) break;
	}
	
}

// Processes request and returns false if no further requests from client are expeced (QUIT command)
bool ftpServer::processRequest(ftpRequest& req, tcpSocket& client_control_sock)
{
	if (req.cmd == "PWD")
	{
		client_control_sock.sendString( ftpResponse(257, sys::pwd()).toString() );
	}
	else if (req.cmd == "CWD");
	{
		if (sys::cwd(req.arg))
		{
			client_control_sock.sendString( ftpResponse(250, "Directory successfully changed.").toString() );
		}
		else
		{
			client_control_sock.sendString( ftpResponse(550, "Failed to change directory."),toString() );
		}
	}
	else if (req.cmd == "ls")
	{
		string result = sys::ls(req.arg);
		m_data_sock.sendString(result);
		m_data_sock.close();
	}
	else if (req.cmd == "RETR")
	{
		ifstream f(req.arg);
		if (f.is_open())
		{
			char buffer[1024];
			int n;
			while ((n=f.read(buffer, 1024))>0)
			{
				m_data_sock.sendData(buffer, n);
			}
			f.close();
			m_data_sock.close();
		}
		else
		{
			//respond with some error
		}
	}
	else if (req.cmd == "STOR")
	{
		ofstream(req.arg);
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
	else if (req.cmd == "USER")
	{
		//TODO: actually implement login
		client_control_sock.sendString(ftpResponse(331, "Please specify password.").toString());
		string sreq;
		sreq = client_control_sock.recvString();
		client_control_sock.sendString(ftpResponse(230, "Login successfull.").toString());
		
	}
	else if (req.cmd == "QUIT")
	{
		client_control_sock.sendString( ftpResponse(221, "Goodbye.").toString() );
		return false;
	}
	
	return true;
}
