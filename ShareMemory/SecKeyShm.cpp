#include "SecKeyShm.h"
#include <string.h>
#include <iostream>
using namespace std;

SecKeyShm::SecKeyShm(int key, int maxNode)
	: BaseShm(key, maxNode * sizeof(NodeSHMInfo))
	, m_maxNode(maxNode)
{
}

SecKeyShm::SecKeyShm(string pathName, int maxNode)
	: BaseShm(pathName, maxNode * sizeof(NodeSHMInfo))
	, m_maxNode(maxNode)
{
}

SecKeyShm::~SecKeyShm()
{
}

void SecKeyShm::shmInit()
{
	if (shmaddr != NULL)
	{
		memset(shmaddr, 0, m_maxNode * sizeof(NodeSHMInfo));
	}
}

int SecKeyShm::shmWrite(NodeSHMInfo* pNodeInfo)
{
	int ret = -1;
	// ���������ڴ�
	NodeSHMInfo* pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL)
	{
		return ret;
	}

	// �жϴ����������Կ�Ƿ��Ѿ�����
	NodeSHMInfo* pNode = NULL;
	for (int i = 0; i < m_maxNode; i++)
	{
		// pNode����ָ��ÿ���ڵ���׵�ַ
		pNode = pAddr + i;
		cout << i << endl;
		cout << "clientID �Ƚ�: " << pNode->clientID << ", " << pNodeInfo->clientID << endl;
		cout << "serverID �Ƚ�: " << pNode->serverID << ", " << pNodeInfo->serverID << endl;
		cout << endl;
		if (strcmp(pNode->clientID, pNodeInfo->clientID) == 0 &&
			strcmp(pNode->serverID, pNodeInfo->serverID) == 0)
		{
			// ����ҵ��˸�������Կ�Ѿ�����, ʹ������Կ���Ǿɵ�ֵ
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			unmapShm();
			cout << "д���ݳɹ�: ԭ���ݱ�����!" << endl;
			return 0;
		}
	}

	// ��û���ҵ���Ӧ����Ϣ, ��һ���սڵ㽫��Կ��Ϣд��
	int i = 0;
	NodeSHMInfo tmpNodeInfo; //�ս��
	for (i = 0; i < m_maxNode; i++)
	{
		pNode = pAddr + i;
		if (memcmp(&tmpNodeInfo, pNode, sizeof(NodeSHMInfo)) == 0)
		{
			ret = 0;
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			cout << "д���ݳɹ�: ���µĽڵ����������!" << endl;
			break;
		}
	}
	if (i == m_maxNode)
	{
		ret = -1;
	}

	unmapShm();
	return ret;
}

NodeSHMInfo SecKeyShm::shmRead(string clientID, string serverID)
{
	int ret = 0;
	// ���������ڴ�
	NodeSHMInfo* pAddr = NULL;
	pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL)
	{
		cout << "�����ڴ����ʧ��..." << endl;
		return NodeSHMInfo();
	}
	cout << "�����ڴ�����ɹ�..." << endl;

	//����������Ϣ
	int i = 0;
	NodeSHMInfo info;
	NodeSHMInfo* pNode = NULL;
	// ͨ��clientID��serverID���ҽڵ�
	cout << "maxNode: " << m_maxNode << endl;
	for (i = 0; i < m_maxNode; i++)
	{
		pNode = pAddr + i;
		cout << i << endl;
		cout << "clientID �Ƚ�: " << pNode->clientID << ", " << clientID.data() << endl;
		cout << "serverID �Ƚ�: " << pNode->serverID << ", " << serverID.data() << endl;
		if (strcmp(pNode->clientID, clientID.data()) == 0 &&
			strcmp(pNode->serverID, serverID.data()) == 0)
		{
			// �ҵ��Ľڵ���Ϣ, ��������������
			info = *pNode;
			cout << "++++++++++++++" << endl;
			cout << info.clientID << " , " << info.serverID << ", "
				<< info.seckeyID << ", " << info.status << ", "
				<< info.seckey << endl;
			cout << "===============" << endl;
			break;
		}
	}

	unmapShm();
	return info;
}
