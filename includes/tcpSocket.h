#ifndef H_TCPSOCKET
#define H_TCPSOCKET

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

class tcpSocket
{
private:
	int m_sd;	//socket file descriptor
	string m_dest_addr;
	unsigned short m_dest_port, m_src_port;
	bool m_passive;
	
	const static int RECV_STR_BUF_SIZE = 1024;
	
public:
	tcpSocket();
	tcpSocket(int sd);
	
	unsigned short getSrcPort();
	
	bool connect(string hostname, unsigned short port);
	bool bind();
	bool bind(unsigned short port);
	bool listen();
	tcpSocket accept();
	
	int send(string data);
	string recvString(int max_bytes);
	string recvString();
	
	int recvData(char* buffer, int size);
	
	void close();
};

#endif
