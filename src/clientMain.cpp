#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <cctype>
#include "../includes/sys.h"
#include "../includes/ftpClient.h"
#include "../includes/ftpResponse.h"
#include "../includes/ftpRequest.h"
#include "../includes/tcpSocket.h"


using namespace std;

int main(int argc, char* argv[])
{
	string hostname;
	unsigned short port = 21;
	
	if(argc<2)
	{
		printf("Specify hostname\n");
		return 0;
	}
	
	hostname = string(argv[1]);
	
	if(argc>2)
	{
		if(strlen(argv[2]) > 5)
		{
			printf("Invalid Port\n");
			return 0; 
		}
		port = 0;
		for(int i=0; i<strlen(argv[2]); i++)
		{
			if(!isdigit(argv[2][i]))
			{
				printf("Invalid Port\n");
				return 0;
			}
			port = port*10 + (argv[2][i] - '0');
		}
	}
	
	ftpClient client(hostname,port,cout);
	
	if(!client.connect())	printf("Unable to connect to Server\n");
	else
	{	
		char username[100],*password;
	
		do
		{
			printf("Enter Username: ");
			fgets(username, 100, stdin);
			username[strlen(username)-1] = '\0';
			client.sendUsername(username);
			password = getpass("Enter Password: ");
		}while(!client.sendPassword(password));
		
		if(!client.setupDataPort())
		{
			printf("Unable to setup data port\n");
			return 0;
		}
		
		string cmd = "";
		char buffer[100];
		
		while(true)
		{
			printf("ftp> ");
			*buffer=0;
			fgets(buffer, 100, stdin);
			cmd = string(strtok(buffer," \t\r\n"));
			
			if(cmd == "pwd")
			{
				client.pwd();
			}
			else if(cmd == "ls")
			{
				char* dir = strtok(NULL,"\n");
				if(dir)	client.ls(dir);
				else client.ls("");
			}
			else if(cmd == "cd")
			{
				char* dir = strtok(NULL,"\n");
				if(dir)	client.cd(dir);
			}
			else if(cmd == "get")
			{
				char* filename = strtok(NULL,"\n");
			
				filebuf fb;
	  			fb.open (filename, ios::out);
	  			ostream os(&fb);
			
				//ofstream f(filename);
				if (fb.is_open())	client.get(filename, os);
				else printf("Unable to create file\n");
				
				fb.close();
			}
			else if(cmd == "put")
			{
				char* filename = strtok(NULL,"\n");
			
				filebuf fb;
	  			fb.open (filename, ios::in);
	  			
	  			istream is(&fb);
	  			
				//ifstream f(filename);
				if (fb.is_open())	client.put(filename, is);
				else 	printf("Unable to open file\n");
				
				fb.close();
			}
			else if(cmd == "quit")
			{
				client.quit();
				break;
			}
			else if(cmd == "!pwd")
			{
				cout << sys::pwd() << endl;
			}
			else if(cmd == "!ls")
			{
				char* dir = strtok(NULL,"\n");
				if(dir)	cout << sys::ls(dir) << endl;
				else cout << sys::ls("") << endl;
			}
			else if(cmd == "!cd")
			{
				char* dir = strtok(NULL,"\n");
				if(dir)
				{
					if(sys::cd(dir))	cout << "Directory successfully changed." << endl;
					else	cout << "Failed to change directory." << endl;
				}
			}
			else
			{
				cout<<"Invalid command"<<endl;
			}		
		}
	}	
	return 0;
}
