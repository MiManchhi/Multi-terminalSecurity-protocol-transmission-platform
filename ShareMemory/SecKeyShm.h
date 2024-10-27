#pragma once
#include "BaseShm.h"
#include<string.h>
class NodeSHMInfo
{
public:
	NodeSHMInfo() : status(0), seckeyID(0)
	{
		bzero(clientID, sizeof(clientID));
		bzero(serverID, sizeof(serverID));
		bzero(seckey, sizeof(seckey));
	}
	int status;		// ��Կ״̬: 1����, 0:������
	int seckeyID;	// ��Կ�ı��
	char clientID[12];	// �ͻ���ID, �ͻ��˵ı�ʶ
	char serverID[12];	// ������ID, ��������ʶ
	char seckey[128];	// �ԳƼ��ܵ���Կ
};

class SecKeyShm : public BaseShm
{
public:
	SecKeyShm(int key, int maxNode);
	SecKeyShm(std::string pathName, int maxNode);
	~SecKeyShm();

	void shmInit();
	int shmWrite(NodeSHMInfo* pNodeInfo);
	NodeSHMInfo shmRead(std::string clientID, std::string serverID);

private:
	int m_maxNode;
};


