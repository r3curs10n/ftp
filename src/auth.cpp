#include "../includes/auth.h"

auth::auth()
{
	cur_user_index = -1;
	
	ifstream fin("auth");
	userInfo user;
	while(fin >>  user.username >> user.password >> user.directory)
	{
		users.push_back(user);
	}
}

bool auth::login(string name, string passwd)
{
	if(name == "anonymous")
	{
		cur_user_index = 0;
		return true;
	}
	for(vector<userInfo>::iterator it = users.begin(); it != users.end(); it++)
	{
		if(it->username == name && it->password == passwd)
		{
			cur_user_index = it - users.begin();
			return true;
		}
	}
	cur_user_index = -1;
	return false;
}

auth::userInfo auth::getCurUser()
{
	if(cur_user_index == -1)	throw string("Invalid user!");
	else	return users[cur_user_index];
}

bool auth::isLoggedin()
{
	return cur_user_index != -1;
}

