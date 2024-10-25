#include "TcpSocket.h"
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

int TcpSocket::connectServer(std::string ip, unsigned short port, int timeout)
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

std::string TcpSocket::recvMsg(int timeout)
{
	// 返回0 -> 没超时就接收到了数据, -1, 超时或有异常
	int ret = readTimeout(timeout);
	if (ret != 0)
	{
		if (ret == -1 || errno == ETIMEDOUT)
		{
			printf("readTimeout(timeout) err: TimeoutError \n");
			return std::string();
		}
		else
		{
			printf("readTimeout(timeout) err: %d \n", ret);
			return std::string();
		}
	}

	int netdatalen = 0;
	ret = readn(&netdatalen, 4); //读包头 4个字节
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return std::string();
	}
	else if (ret < 4)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return std::string();
	}

	int n = ntohl(netdatalen);
	// 根据包头中记录的数据大小申请内存, 接收数据
	char* tmpBuf = (char*)malloc(n + 1);
	if (tmpBuf == NULL)
	{
		ret = MallocError;
		printf("malloc() err \n");
		return NULL;
	}

	ret = readn(tmpBuf, n); //根据长度读数据
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return std::string();
	}
	else if (ret < n)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return std::string();
	}

	tmpBuf[n] = '\0'; //多分配一个字节内容，兼容可见字符串 字符串的真实长度仍然为n
	std::string data = std::string(tmpBuf);
	// 释放内存
	free(tmpBuf);

	return data;
}

int TcpSocket::sendMsg(std::string sendData, int timeout)
{
	// 返回0->没超时, 返回-1->超时
	int ret = writeTimeout(timeout);
	if (ret == 0)
	{
		int writed = 0;
		int dataLen = sendData.size() + 4;
		// 添加的4字节作为数据头, 存储数据块长度
		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == NULL)
		{
			ret = MallocError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		// 转换为网络字节序
		int netlen = htonl(sendData.size());
		memcpy(netdata, &netlen, 4);
		memcpy(netdata + 4, sendData.data(), sendData.size());

		// 没问题返回发送的实际字节数, 应该 == 第二个参数: dataLen
		// 失败返回: -1
		writed = writen(netdata, dataLen);
		if (writed < dataLen)	// 发送失败
		{
			if (netdata != NULL)
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}
		// 释放内存
		if (netdata != NULL)
		{
			free(netdata);
			netdata = NULL;
		}
	}
	else
	{
		//失败返回-1，超时返回-1并且errno = ETIMEDOUT
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = TimeoutError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
		}
	}

	return ret;
}

int TcpSocket::connectTimeout(struct sockaddr_in* addr, unsigned int wait_seconds)
{
	int ret = 0;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	//设置非阻塞
	if (wait_seconds > 0)
	{
		setNonBlock(sock_cfd);
	}
	//非阻塞下调用connect
	ret = connect(sock_cfd, (struct sockaddr*)addr, addrlen);
	//select检测文件描述符是否可写
	if (ret < 0 && errno == EINPROGRESS)
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sock_cfd, &set);
		struct timeval outtime{wait_seconds,0};
		do
		{
			ret = select(sock_cfd + 1, NULL, &set, NULL, &outtime);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
		{
			// 超时
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret < 0)
		{
			return -1;
		}
		else if (ret == 1)
		{
			/* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
			/* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
			int err;
			socklen_t sockLen = sizeof(err);
			int sockoptret = getsockopt(sock_cfd, SOL_SOCKET, SO_ERROR, &err, &sockLen);
			if (sockoptret == -1)
			{
				return -1;
			}
			if (err == 0)
			{
				ret = 0;	// 成功建立连接
			}
			else
			{
				// 连接失败
				errno = err;
				ret = -1;
			}
		}
	}
	if (wait_seconds > 0)
	{
		setBlock(sock_cfd);
	}
	return ret;
}

int TcpSocket::setBlock(int fd)
{
	int flag = fcntl(fd, F_GETFL);
	if (flag == -1)
	{
		return flag;
	}
	flag &= ~O_NONBLOCK;
	int ret = fcntl(fd, F_SETFL,flag);
	return ret;
}

int TcpSocket::setNonBlock(int fd)
{
	int flag = fcntl(fd, F_GETFL);
	if (flag == -1)
	{
		return flag;
	}
	flag |= O_NONBLOCK;
	int ret = fcntl(fd, F_SETFL,flag);
	return ret;
}

void TcpSocket::closeConnect()
{
	if (sock_cfd >= 0)
	{
		close(sock_cfd);
	}
}
int TcpSocket::writen(const void* buf, int count)
{
	size_t nleft = count;
	ssize_t nwritten;
	char* bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nwritten = write(sock_cfd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)	// 被信号打断
			{
				continue;
			}
			return -1;
		}
		else if (nwritten == 0)
		{
			continue;
		}

		bufp += nwritten;
		nleft -= nwritten;
	}

	return count;
}
int TcpSocket::readn(void* buf, int count)
{
	size_t nleft = count;
	ssize_t nread;
	char* bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nread = read(sock_cfd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if (nread == 0)
		{
			return count - nleft;
		}

		bufp += nread;
		nleft -= nread;
	}

	return count;
}
int TcpSocket::readTimeout(unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sock_cfd, &set);
		struct timeval outtime { wait_seconds, 0 };
		do
		{
			ret = select(sock_cfd + 1, &set, NULL, NULL, &outtime);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
		{
			ret = 0;
		}
		return ret;
	}
}
int TcpSocket::writeTimeout(unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sock_cfd, &set);
		struct timeval outtime{wait_seconds,0};
		do
		{
			ret = select(sock_cfd + 1, NULL, &set, NULL, &outtime);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
		{
			ret = 0;
		}
		return ret;
	}
}
TcpSocket::~TcpSocket()
{

}