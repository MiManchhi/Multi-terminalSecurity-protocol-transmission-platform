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
TcpSocket* TcpServer::acceptConn(int timeout = TIMEOUT)
{

}
int TcpServer::setListen(unsigned short port)
{
	int ret = 0;
	//创建监听套接字
	sock_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_lfd == -1)
	{
		ret = errno;
		return ret;
	}
	//设置服务器信息
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	//设置端口复用
	int opt = 1;
	setsockopt(sock_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}
	//绑定
	bind(sock_lfd, (struct sockaddr*)&server, sizeof(server));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}
	//监听
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