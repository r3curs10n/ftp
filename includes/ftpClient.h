#ifndef H_FTPCLIENT
#define H_FTPCLIENT

#include "ftpResponse.h"
#include "ftpRequest.h"
#include "tcpSocket.h"
#include "sys.h"
#include <ostream>

#define FILE_BLOCK_SIZE 1024

using namespace std;

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
	void setDataPort(unsigned short data_port);
	void setServerPort(unsigned short server_port);
	void setServerName(string server_hostname);
	unsigned short getDataPort();
	unsigned short getServerPort();
	string getServerName();
	tcpSocket getDataSocket();
	tcpSocket getControlSocket();
	ostream& getLog();
	bool setupDataPort();
	bool sendRequest(ftpRequest request);
	ftpResponse recvResponse();
	bool connect();
	void sendUsername(string username);
	bool sendPassword(string password);
	void pwd();
	void cd(string pathname);
	void ls(string dir);
	bool get(string filename, ostream& f);
	bool put(string filename, istream& f);
	void quit();
};

#endif
