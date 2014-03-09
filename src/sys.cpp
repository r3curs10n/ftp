#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <iostream>

using namespace std;

namespace sys
{
	string pwd()
	{
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		return string(cwd);
	}
	
	int cd(string dir = "")
	{
		return chdir(dir.c_str());
	}
	
	string ls(string arg = "")
	{
		string cmd = "ls";
		if(dir != "")	cmd += " " + arg;
		FILE* file = popen(cmd.c_str(), "r");
		// use fscanf to read:
		char buffer[100];
		string fileList;
		while(fscanf(file, "%100s", buffer)!=EOF)
		{
			fileList += string(buffer) + "\t";
		}
		pclose(file);
		return fileList;
	}
	
	string syst()
	{
		return string("UNIX");
	}
}
