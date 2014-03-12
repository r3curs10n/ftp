#ifndef H_AUTH
#define H_AUTH

#include <vector>
#include <fstream>
#include <string>

using namespace std;

class auth
{
	struct userInfo
	{
		string username;
		string password;
		string directory;
	};
	
	vector<userInfo> users;
	int cur_user_index;
	
public:
	auth();
	bool login(string name, string passwd);
	userInfo getCurUser();
	bool isLoggedin();
};

#endif


