#include "../includes/ftpResponse.h"
#include <sstream>

ftpResponse::ftpResponse() {}

ftpResponse::ftpResponse(int code, string msg): 
	m_code(code), 
	m_msg(msg) 
{}

int ftpResponse::getCode()
{
	return m_code;
}

string ftpResponse::getMessage()
{
	return m_msg;
}

void ftpResponse::setCode(int code)
{
	m_code = code;
}

void ftpResponse::setMessage(string msg)
{
	m_msg = msg;
}

/// input: string containing response from socket
/// output: ftpResponse object
ftpResponse ftpResponse::parseFtpResponse(string s)
{
	int code;
	string msg;
	
	for (int i=0; i<s.length(); i++)
	{
		//split at space
		if (s[i]==' ')
		{
			code = atoi(s.substr(0, i).c_str());
			int j;
			//end at "\r\n"
			for (j=i+2; j<s.length() && !(s[j-1]=='\r' && s[j]=='\n'); j++);
			msg = s.substr(i+1, j-1 - (i+1));
			break;
		}
	}
	
	return ftpResponse(code, msg);
}

string ftpResponse::toString()
{
	stringstream s;
	s<<m_code<<" "<<m_msg<<"\r\n";
	return s.str();
}
