#include <cstdio>
#include "TcpSocket.h"
#include <string.h>
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
	// �����׽���
	TcpSocket socket;
	// ���ӷ�����
	cout << "��ʼ����...";
	socket.connectServer("127.0.0.1", 9999);
	cout << "���ӳɹ�...";

	// ͨ��
	while (1)
	{
		string sendmsg = "hello server ...";
		cout << "��������" << sendmsg << endl;
		socket.sendMsg(sendmsg);
		// ��������
		int recvLen = -1;
		string recvMsg = socket.recvMsg();
		cout << "recvmsg: " << recvMsg << endl;
		sleep(1);
	}
	return 0;
}