#ifndef H_SYS
#define H_SYS

#include <string>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>

using namespace std;

namespace sys
{
	string pwd()
	{
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		return string(cwd);
		
		/*string cmd = "pwd";
		FILE* file = popen(cmd.c_str(), "r");
		// use fscanf to read:
		char buffer[105];
		string fileList;
		int n;
		while(!feof(file))
		{
			fgets(buffer, 100, file);
			n = strlen(buffer);
			buffer[n] = '\0';
			fileList += string(buffer);
		}
		cout<<">>Current Dir: \n"<<fileList<<endl;
		pclose(file);*/
	}
	
	int cd(string dir = "")
	{
		cout << dir << endl;
		return chdir(dir.c_str());
	}
	
	int setRootDir(string dir)
	{
		cout << dir << endl;
		return chroot(dir.c_str());
	}
	
	string ls(string arg = "")
	{
		string cmd = "ls";
		if(arg != "")	cmd += " " + arg;
		FILE* file = popen(cmd.c_str(), "r");
		// use fscanf to read:
		char buffer[105];
		stringstream fileList;
		int n;
		while((n=fread(buffer, 1, 100, file))>0)
		{
			for (int i=0; i<n; i++)
			{
				if (buffer[i]=='\n') fileList<<'\r';
				fileList<<buffer[i];
			}
		}
		pclose(file);
		
		return fileList.str();
		
		/*DIR Dir;
		struct dirent *DirEntry;
		Dir = opendir(arg.c_str());
		
		stringstream ss;
		
		while(Dir=readdir(Dir))
		{
		   ss << DirEntry->d_name;
		}
		
		return ss.str();*/
	}
	
	string syst()
	{
		return string("UNIX");
	}
}

#endif


