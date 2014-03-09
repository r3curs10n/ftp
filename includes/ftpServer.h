#ifndef H_FTPSERVER
#define H_FTPSERVER

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include "ftpRequest.h"
#include "ftpResponse.h"
#include "tcpSocket.h"

class ftpServer
{
private:
	unsigned short m_port;
	tcpSocket m_control_sock;
	tcpSocket m_data_sock;
public:
	ftpServer(int port);
	
	bool start();
	void serveClient(tcpSocket);
	void processRequest(ftpRequest&, tcpSocket&);
	
};

#endif
