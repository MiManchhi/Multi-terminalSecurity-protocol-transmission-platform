#pragma once
#include<iostream>
#include "TcpSocket.h"
class TcpServer
{
public:
	//默认构造
	TcpServer();
	//析构
	~TcpServer();
	//设置监听
	int setListen(unsigned short port);
	//接收客户端连接
	TcpSocket* acceptConn(int timeout = TIMEOUT);
	//关闭
	void closefd();
private:
	int sock_lfd;  //用于监听的文件描述符
};

