#ifndef H_FTPSERVER
#define H_FTPSERVER

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include "ftpRequest.h"
#include "ftpResponse.h"
#include "tcpSocket.h"
#include "auth.h"

class ftpServer
{
private:
	unsigned short m_port;
	tcpSocket m_control_sock;
	tcpSocket m_data_sock;
	auth Auth;
public:
	ftpServer(int port);
	
	bool start();
	void serveClient(tcpSocket);
	bool processRequest(ftpRequest&, tcpSocket&);
	
};

#endif
