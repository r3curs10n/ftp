#include "../includes/ftpClient.h"
#include "../includes/ftpResponse.h"
#include <thread>

ftpClient::ftpClient(string server_hostname, unsigned short server_port)
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
	
	sendRequest(ftpRequest("USER", username));
	ftpResponse response = recvResponse();
	m_log << response.msg << endl;
	
	sendRequest(ftpRequest("PASS", password));
	response = recvResponse();	
	m_log << response.msg << endl;
	
	return true;
}

void ftpClient::pwd()
{
	sendRequest("PWD");
	ftpResponse response = recvResponse();
	m_log << response.msg << endl;
}

void ftpClient::cd(string pathname)
{
	sendRequest("CWD", pathname);
	ftpResponse response = recvResponse();
	m_log << response.msg << endl;
}

void ftpClient::ls()
{
	sendRequest(ftpRequest("PORT",m_data_port));
	ftpResponse response = recvResponse();
	m_log << response.msg << endl;
	
	sendRequest(ftpRequest("LIST"));
	response = recvResponse();
	m_log << response.msg << endl;
	
	string s;
	tcpSocket cur_data_socket = m_data_socket.accept();
	while((s = m_data_socket.recvString()).length() > 0)
	{
		m_log << s << endl;
	}
	cur_data_socket.close();
	
	response = recvResponse();
	m_log << response.msg << endl;
}

