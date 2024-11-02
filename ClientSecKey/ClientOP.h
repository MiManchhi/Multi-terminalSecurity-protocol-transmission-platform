#pragma once
#include <string>
using namespace std;

struct ClientInfo
{
	string ServerID;
	string ClientID;
	string ip;
	unsigned short port;
};

class ClientOP
{
public:
	ClientOP(string jsonFile);
	~ClientOP();

	// ��ԿЭ��
	bool seckeyAgree();

	// ��ԿУ��
	void seckeyCheck();

	// ��Կע��
	void seckeyCancel();

private:
	ClientInfo m_info;
};

