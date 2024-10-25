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
		// ��ʱ
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = TimeoutError;
			return ret;
		}
		else
		{
			printf("connectTimeout �����쳣, �����: %d\n", errno);
			return errno;
		}
	}

	return ret;
}

std::string TcpSocket::recvMsg(int timeout)
{
	// ����0 -> û��ʱ�ͽ��յ�������, -1, ��ʱ�����쳣
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
	ret = readn(&netdatalen, 4); //����ͷ 4���ֽ�
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
	// ���ݰ�ͷ�м�¼�����ݴ�С�����ڴ�, ��������
	char* tmpBuf = (char*)malloc(n + 1);
	if (tmpBuf == NULL)
	{
		ret = MallocError;
		printf("malloc() err \n");
		return NULL;
	}

	ret = readn(tmpBuf, n); //���ݳ��ȶ�����
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

	tmpBuf[n] = '\0'; //�����һ���ֽ����ݣ����ݿɼ��ַ��� �ַ�������ʵ������ȻΪn
	std::string data = std::string(tmpBuf);
	// �ͷ��ڴ�
	free(tmpBuf);

	return data;
}

int TcpSocket::sendMsg(std::string sendData, int timeout)
{
	// ����0->û��ʱ, ����-1->��ʱ
	int ret = writeTimeout(timeout);
	if (ret == 0)
	{
		int writed = 0;
		int dataLen = sendData.size() + 4;
		// ��ӵ�4�ֽ���Ϊ����ͷ, �洢���ݿ鳤��
		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == NULL)
		{
			ret = MallocError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		// ת��Ϊ�����ֽ���
		int netlen = htonl(sendData.size());
		memcpy(netdata, &netlen, 4);
		memcpy(netdata + 4, sendData.data(), sendData.size());

		// û���ⷵ�ط��͵�ʵ���ֽ���, Ӧ�� == �ڶ�������: dataLen
		// ʧ�ܷ���: -1
		writed = writen(netdata, dataLen);
		if (writed < dataLen)	// ����ʧ��
		{
			if (netdata != NULL)
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}
		// �ͷ��ڴ�
		if (netdata != NULL)
		{
			free(netdata);
			netdata = NULL;
		}
	}
	else
	{
		//ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
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
	//���÷�����
	if (wait_seconds > 0)
	{
		setNonBlock(sock_cfd);
	}
	//�������µ���connect
	ret = connect(sock_cfd, (struct sockaddr*)addr, addrlen);
	//select����ļ��������Ƿ��д
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
			// ��ʱ
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret < 0)
		{
			return -1;
		}
		else if (ret == 1)
		{
			/* ret����Ϊ1����ʾ�׽��ֿ�д�������������������һ�������ӽ����ɹ���һ�����׽��ֲ�������*/
			/* ��ʱ������Ϣ���ᱣ����errno�����У���ˣ���Ҫ����getsockopt����ȡ�� */
			int err;
			socklen_t sockLen = sizeof(err);
			int sockoptret = getsockopt(sock_cfd, SOL_SOCKET, SO_ERROR, &err, &sockLen);
			if (sockoptret == -1)
			{
				return -1;
			}
			if (err == 0)
			{
				ret = 0;	// �ɹ���������
			}
			else
			{
				// ����ʧ��
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
			if (errno == EINTR)	// ���źŴ��
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