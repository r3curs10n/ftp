#ifndef H_FTPREQUEST
#define H_FTPREQUEST

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

class ftpRequest
{
private:
	string m_cmd;
	string m_arg;

public:

	ftpRequest();
	ftpRequest(string cmd, string arg);
	static ftpRequest parseFtpRequest(string s);
	string toString();
	string getCmd();
	string getArg();
	void setCmd(string cmd);
	void setArg(string arg);
	/// input: xx.yy.zz.ww:1234
	/// output: xx,yy,zz,ww,high_byte(1234),low_byte(1234)
	static string splitPortArg(string portArg);

	
	/// input: xx,yy,zz,ww,high_byte(1234),low_byte(1234)
	/// output: xx.yy.zz.ww:1234
	static string combinePortArg(string portArg);
	
};

#endif
