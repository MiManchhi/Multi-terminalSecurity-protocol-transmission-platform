#pragma once
#include<iostream>
#include "TcpSocket.h"
class TcpServer
{
public:
	//Ĭ�Ϲ���
	TcpServer();
	//����
	~TcpServer();
	//���ü���
	int setListen(unsigned short port);
	//���տͻ�������
	TcpSocket* acceptConn(int timeout = TIMEOUT);
	//�ر�
	void closefd();
private:
	int sock_lfd;  //���ڼ������ļ�������
};

