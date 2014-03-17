#include "../includes/ftpClient.h"

ftpClient::ftpClient(string server_hostname, unsigned short server_port, ostream& log):
	m_log(log)
{
	m_server_hostname = server_hostname;
	m_server_port = server_port;
}

void ftpClient::setServerPort(unsigned short server_port)
{
	m_server_port = server_port;
}

void ftpClient::setDataPort(unsigned short data_port)
{
	m_data_port = data_port;
}

void ftpClient::setServerName(string server_hostname)
{
	m_server_hostname = server_hostname;
}

unsigned short ftpClient::getDataPort()
{
	return m_data_port;
}

unsigned short ftpClient::getServerPort()
{
	return m_server_port;
}

string ftpClient::getServerName()
{
	return m_server_hostname;
}

tcpSocket ftpClient::getDataSocket()
{
	return m_data_socket;
}

tcpSocket ftpClient::getControlSocket()
{
	return m_control_socket;
}

ostream& ftpClient::getLog()
{
	return m_log;
}

bool ftpClient::setupDataPort()
{
	if(m_data_socket.bind() && m_data_socket.listen())
	{
		m_data_port = m_data_socket.getSrcPort();
		return true;
	}
	return false;
};

bool ftpClient::sendRequest(ftpRequest request)
{
	return (m_control_socket.sendString(request.toString())>0);
}

ftpResponse ftpClient::recvResponse()
{
	return ftpResponse::parseFtpResponse(m_control_socket.recvString());
}

bool ftpClient::connect()
{
	if(!m_control_socket.connect(m_server_hostname,m_server_port))
	{
		ftpResponse response = recvResponse();
		cout << response.getMessage() << endl;
		return false;
	}
	else
	{
		ftpResponse response = recvResponse();
		cout << response.getMessage() << endl;
		return true;
	}
}


void ftpClient::sendUsername(string username)
{
	sendRequest(ftpRequest(string("USER"), username));
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
}

bool ftpClient::sendPassword(string password)
{
	sendRequest(ftpRequest(string("PASS"), password));
	ftpResponse response = recvResponse();	
	cout << response.getMessage() << endl;
	
	if(response.getCode() == 230)	return true;
	else return false;
}

void ftpClient::pwd()
{
	sendRequest(ftpRequest(string("PWD")));
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
}

void ftpClient::cd(string pathname)
{
	sendRequest(ftpRequest(string("CWD"), pathname));
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
}

void ftpClient::ls(string dir)
{
	stringstream clientInfo;
	
	clientInfo << m_control_socket.getSrcHostname() << ":" << m_data_port;
	
	sendRequest(ftpRequest(string("PORT"),clientInfo.str()));
	tcpSocket cur_data_socket = m_data_socket.accept();
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
	
	if(response.getCode() != 200)	return;
	
	sendRequest(ftpRequest(string("LIST ") + dir));
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	string s;
	
	while((s = cur_data_socket.recvString()).length() > 0)
	{
		//cout << s << endl;
		printf("%s", s.c_str());
	}
	cur_data_socket.close();
	
	response = recvResponse();
	cout << response.getMessage() << endl;
}

bool ftpClient::get(string filename, ostream& f)
{
	
	sendRequest(ftpRequest(string("TYPE"), string("I")));
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
	
	stringstream clientInfo;
	
	clientInfo << m_control_socket.getSrcHostname() << ":" << m_data_port;
	
	sendRequest(ftpRequest(string("PORT"), clientInfo.str()));
	tcpSocket cur_data_socket = m_data_socket.accept();
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	if(response.getCode() != 200)	return false;
	
	sendRequest(ftpRequest(string("RETR"), filename));
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	if(response.getCode() != 150)	return false;
	
	string s;
	
	char buffer[FILE_BLOCK_SIZE];
	int bytes_received;
	
	while((bytes_received = cur_data_socket.recvData(buffer,FILE_BLOCK_SIZE)) > 0)
	{
		f.write(buffer, bytes_received);
	}
	
	//cur_data_socket.close();
	
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	return true;
}

bool ftpClient::put(string filename, istream& f)
{
	
	if (!sys::isRegularFile(filename))
	{
		cout << "File not present." << endl;
		return false;
	}
	
	sendRequest(ftpRequest(string("TYPE"), string("I")));
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
	
	stringstream clientInfo;
	
	clientInfo << m_control_socket.getSrcHostname() << ":" << m_data_port;
	
	sendRequest(ftpRequest(string("PORT"), clientInfo.str()));
	tcpSocket cur_data_socket = m_data_socket.accept();
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	if(response.getCode() != 200)	return false;
	
	sendRequest(ftpRequest(string("STOR"), filename));
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	if(response.getCode() != 150)	return false;
	
	string s;
	
	char buffer[FILE_BLOCK_SIZE];
	
	while(!f.eof())
	{
		f.read(buffer, FILE_BLOCK_SIZE);
		cur_data_socket.sendData(buffer, f.gcount());
	}
	
	cur_data_socket.close();
	
	response = recvResponse();
	cout << response.getMessage() << endl;
	
	return true;
}

void ftpClient::quit()
{
	sendRequest(ftpRequest(string("QUIT")));
	ftpResponse response = recvResponse();
	cout << response.getMessage() << endl;
}
