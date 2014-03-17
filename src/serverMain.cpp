#include <cstring>
#include <cctype>
#include <cstdio>
#include "../includes/ftpServer.h"
using namespace std;

int main(int argc, char* argv[])
{
	unsigned short port;
	if(argc < 2)
	{
		port = 21;
	}
	else
	{
		if(strlen(argv[1]) > 5)
		{
			printf("Invalid Port\n");
			return 0; 
		}
		port = 0;
		for(int i=0; i<strlen(argv[1]); i++)
		{
			if(!isdigit(argv[1][i]))
			{
				printf("Invalid Port\n");
				return 0;
			}
			port = port*10 + (argv[1][i] - '0');
		}
	}
	
	ftpServer serv = ftpServer(port);
	if (!serv.start()){
		cout<<"Failed to start server. Try another port."<<endl;
	}
	
	return 0;
}

