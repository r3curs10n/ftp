#ifndef H_FTPRESPONSE
#define H_FTPRESPONSE

#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

class ftpResponse
{

private:
	int m_code;
	string m_msg;

public:

	ftpResponse();
	ftpResponse(int code, string msg);

	int getCode();
	string getMessage();
	
	void setCode(int code);
	void setMessage(string msg);

	/// input: string containing response from socket
	/// output: ftpResponse object
	static ftpResponse parseFtpResponse(string s);
	
	string toString();

};

#endif
