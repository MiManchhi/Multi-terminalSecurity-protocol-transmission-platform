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

//超时的时间
static const int TIMEOUT = 10000;
class TcpSocket
{
public:
	enum ErrorType { ParamError = 3001, TimeoutError, PeerCloseError, MallocError };
	//默认构造，用于客户端通信
	TcpSocket();
	//有参构造接收服务器传来的通信文件描述符，进行通信
	TcpSocket(int cfd);
	//连接服务器
	int connectServer(std::string ip, unsigned short port, int timeout = TIMEOUT);
	//发送数据
	int sendMsg(std::string sendData, int timeout = TIMEOUT);
	//接收数据
	std::string recvMsg(int timeout = TIMEOUT);
	//关闭连接
	void closeConnect();
	~TcpSocket();
private:
	// 设置I/O为非阻塞模式
	int setNonBlock(int fd);
	// 设置I/O为阻塞模式
	int setBlock(int fd);
	// 读超时检测函数，不含读操作
	int readTimeout(unsigned int wait_seconds);
	// 写超时检测函数, 不包含写操作
	int writeTimeout(unsigned int wait_seconds);
	// 带连接超时的connect函数
	int connectTimeout(struct sockaddr_in* addr, unsigned int wait_seconds);
	// 每次从缓冲区中读取n个字符
	int readn(void* buf, int count);
	// 每次往缓冲区写入n个字符
	int writen(const void* buf, int count);
private:
	int sock_cfd;  //用于通信的socket
};

