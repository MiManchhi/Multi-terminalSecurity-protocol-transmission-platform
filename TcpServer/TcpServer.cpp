#include "TcpServer.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

TcpServer::TcpServer()
{

}
TcpServer::~TcpServer()
{

}
TcpSocket* TcpServer::acceptConn(int timeout)
{
	//��ʱ���
	int ret = 0;
	if (timeout > 0)
	{
		//�������ļ��������Ž�����
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sock_lfd, &set);
		struct timeval outtime{timeout,0};
		//select����ļ��������Ƿ�ɶ�
		do
		{
			ret = select(sock_lfd + 1, &set, NULL, NULL, &outtime);
		} while (ret < 0 && errno == EINTR);
		if (ret <= 0)
		{
			return NULL;
		}
		//��ʱһ�����տͻ�������  ���Ҳ�������
		struct sockaddr_in client;
		memset(&client, 0, sizeof(struct sockaddr_in));
		socklen_t clientlen = sizeof(struct sockaddr_in);
		int connfd = accept(sock_lfd, (struct sockaddr*)&client, &clientlen);
		if (connfd == -1)
		{
			return NULL;
		}
		else
		{
			return new TcpSocket(connfd);
		}
	}
}
int TcpServer::setListen(unsigned short port)
{
	int ret = 0;
	//���������׽���
	sock_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_lfd == -1)
	{
		ret = errno;
		return ret;
	}
	//���÷�������Ϣ
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	//���ö˿ڸ���
	int opt = 1;
	setsockopt(sock_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}
	//��
	bind(sock_lfd, (struct sockaddr*)&server, sizeof(server));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}
	//����
	listen(sock_lfd, 128);
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}
	return ret;
}
void TcpServer::closefd()
{
	close(sock_lfd);
}