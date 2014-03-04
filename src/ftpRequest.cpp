#include "../includes/ftpRequest.h"


ftpRequest::ftpRequest() {}

ftpRequest::ftpRequest(string cmd, string arg)
{
	m_cmd = cmd;
	m_arg =  arg;
}

ftpRequest ftpRequest::parseFtpRequest(string s)
{
	int i = 0;
	string cmd,arg;
	cmd = "";
	while(s[i]!=' ')
	{
		cmd += s[i];
		i++;
	}
	if(s=="PWD" || s=="QUIT")
	{
		arg = "";
	}
	else
	{
		i++;
		if(s=="PORT")
		{
			arg = combinePortArg(s.substr(i,s.length()-i));
		}
		else
		{
			arg = s.substr(i,s.length()-i);
		}
	}
	return ftpRequest(cmd,arg);
}

string ftpRequest::toString()
{
	if(m_cmd == "PWD" || m_cmd == "QUIT")
	{
		return m_cmd + "\r\n";
	}
	else if(m_cmd == "PORT")
	{
		return m_cmd + " " + splitPortArg(m_arg) + "\r\n";
	}
	else
	{
		return m_cmd + " " + m_arg + "\r\n";
	}
}

/// input: xx.yy.zz.ww:1234
/// output: xx,yy,zz,ww,high_byte(1234),low_byte(1234)
string ftpRequest::splitPortArg(string portArg)
{
	int port;
	stringstream convert;
	for(int i=0;i<portArg.length();i++)
	{
		if(portArg[i]=='.')	portArg[i] = ',';
		if(portArg[i]==':')
		{
			portArg[i] = ',';
			port = atoi(portArg.substr(i+1,portArg.length()-1-i).c_str());
			portArg = portArg.substr(0,i+1);
			convert << port/256 << "," << port%256;
			portArg += convert.str();
			break;
		}
	}
	return portArg;
}

/// input: xx,yy,zz,ww,high_byte(1234),low_byte(1234)
/// output: xx.yy.zz.ww:1234
string ftpRequest::combinePortArg(string portArg)
{
	int cnt = 0,port = 0;
	stringstream convert;
	for(int i=0;i<portArg.length();i++)
	{
		if(portArg[i]==',')
		{
			cnt++;
			if(cnt < 4)
			{
				portArg[i] = '.';
			}
			else if(cnt == 4)
			{
				portArg[i] = ':';
				for(int j=i+1;j<portArg.length();j++)
				{
					if(portArg[j]==',')
					{
						port *= 256;
					}
					else
					{
						port = port*10 + (portArg[j]-'0');
					}
				}
				portArg = portArg.substr(0,i+1);
				convert << port;
				portArg += convert.str();
				break;
			}
		}
	}
	return portArg;
}

string ftpRequest::getCmd()
{
	return m_cmd; 
}

string ftpRequest::getArg()
{
	return m_arg; 
}

void ftpRequest::setCmd(string cmd)
{
	m_cmd = cmd; 
}

void ftpRequest::setArg(string arg)
{
	m_arg = arg; 
}

