#include "TcpSocket.h"
#include<string>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

TcpSocket::TcpSocket()
{
}

TcpSocket::TcpSocket(int cfd)
{
	sock_cfd = cfd;
}

int TcpSocket::connectServer(std::string ip, unsigned short port, int timeout = TIMEOUT)
{
	int ret = 0;
	if (port <= 0 || port > 65535 || timeout < 0)
	{
		ret = ParamError;
		return ret;
	}
	sock_cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_cfd < 0)
	{
		ret = errno;
		printf("func socket() err:  %d\n", ret);
		return ret;
	}
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip.data());

	ret = connectTimeout((struct sockaddr_in*)(&server), (unsigned int)timeout);
	if (ret < 0)
	{
		// 超时
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = TimeoutError;
			return ret;
		}
		else
		{
			printf("connectTimeout 调用异常, 错误号: %d\n", errno);
			return errno;
		}
	}

	return ret;
}

std::string TcpSocket::recvMsg(int timeout = TIMEOUT)
{

}

int TcpSocket::sendMsg(std::string sendData, int timeout = TIMEOUT)
{


}

void TcpSocket::closeConnect()
{
	if (sock_cfd >= 0)
	{
		close(sock_cfd);
	}
}

TcpSocket::~TcpSocket()
{

}