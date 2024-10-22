#pragma once
#include<iostream>
//#include<stdio.h>
//#include<stdlib.h>
//#include<sys/types.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
//#include<netinet/ip.h>
//#include<unistd.h>
//#include<arpa/inet.h>

//��ʱ��ʱ��
static const int TIMEOUT = 10000;
class TcpSocket
{
public:
	enum ErrorType { ParamError = 3001, TimeoutError, PeerCloseError, MallocError };
	//Ĭ�Ϲ��죬���ڿͻ���ͨ��
	TcpSocket();
	//�вι�����շ�����������ͨ���ļ�������������ͨ��
	TcpSocket(int cfd);
	//���ӷ�����
	int connectServer(std::string ip, unsigned short port, int timeout = TIMEOUT);
	//��������
	int sendMsg(std::string sendData, int timeout = TIMEOUT);
	//��������
	std::string recvMsg(int timeout = TIMEOUT);
	//�ر�����
	void closeConnect();
	~TcpSocket();
private:
	// ����I/OΪ������ģʽ
	int setNonBlock(int fd);
	// ����I/OΪ����ģʽ
	int setBlock(int fd);
	// ����ʱ��⺯��������������
	int readTimeout(unsigned int wait_seconds);
	// д��ʱ��⺯��, ������д����
	int writeTimeout(unsigned int wait_seconds);
	// �����ӳ�ʱ��connect����
	int connectTimeout(struct sockaddr_in* addr, unsigned int wait_seconds);
	// ÿ�δӻ������ж�ȡn���ַ�
	int readn(void* buf, int count);
	// ÿ����������д��n���ַ�
	int writen(const void* buf, int count);
private:
	int sock_cfd;  //����ͨ�ŵ�socket
};

