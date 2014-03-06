#include "../includes/ftpClient.h"

ftpClient::ftpClient(string server_hostname, unsigned short server_port, ostream& log):
	m_log(log)
{
	m_server_hostname = server_hostname;
	m_server_port = server_port;
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
	return (m_control_socket.send(request.toString())>0);
}

ftpResponse ftpClient::recvResponse()
{
	return ftpResponse::parseFtpResponse(m_control_socket.recvString());
}

bool ftpClient::connect(string username, string password)
{
	if(!m_control_socket.connect(m_server_hostname,m_server_port))
	{
		m_log << "Connection Failed!" << endl;
		return false;
	}
	
	sendRequest(ftpRequest(string("USER"), username));
	ftpResponse response = recvResponse();
	m_log << response.getMessage() << endl;
	
	sendRequest(ftpRequest(string("PASS"), password));
	response = recvResponse();	
	m_log << response.getMessage() << endl;
	
	return true;
}

void ftpClient::pwd()
{
	sendRequest(ftpRequest(string("PWD")));
	ftpResponse response = recvResponse();
	m_log << response.getMessage() << endl;
}

void ftpClient::cd(string pathname)
{
	sendRequest(ftpRequest(string("CWD"), pathname));
	ftpResponse response = recvResponse();
	m_log << response.getMessage() << endl;
}

void ftpClient::ls()
{

	stringstream clientInfo;
	
	clientInfo << m_data_socket.getSrcHostname() << ":" << m_data_port;
	
	sendRequest(ftpRequest(string("PORT"),clientInfo.str()));
	ftpResponse response = recvResponse();
	m_log << response.getMessage() << endl;
	
	sendRequest(ftpRequest(string("LIST")));
	response = recvResponse();
	m_log << response.getMessage() << endl;
	
	string s;
	tcpSocket cur_data_socket = m_data_socket.accept();
	while((s = cur_data_socket.recvString()).length() > 0)
	{
		m_log << s << endl;
	}
	cur_data_socket.close();
	
	response = recvResponse();
	m_log << response.getMessage() << endl;
}

