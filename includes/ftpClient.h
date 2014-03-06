#ifndef H_FTPCLIENT
#define H_FTPCLIENT

#include "ftpClient.h"
#include "ftpResponse.h"
#include "ftpRequest.h"
#include "tcpSocket.h"
#include <ostream>

class ftpClient
{
private:
	unsigned short m_data_port;
	unsigned short m_server_port;
	string m_server_hostname;
	tcpSocket m_control_socket;
	tcpSocket m_data_socket;
	ostream& m_log;
public:
	ftpClient(string server_hostname, unsigned short server_port, ostream& log);
	bool setupDataPort();
	bool sendRequest(ftpRequest request);
	ftpResponse recvResponse();
	bool connect(string username, string password);
	void pwd();
	void cd(string pathname);
	void ls();
};

#endif
