#pragma once
#include <map>
#include <condition_variable>
#include <mutex>
#include "TcpServer.h"
#include "msg.pb.h"

using namespace std;
// 处理客户端请求
class ServerOP
{
public:
	enum KeyLen { Len16 = 16, Len24 = 24, Len32 = 32 };
	ServerOP(string json);
	// 启动服务器
	void startServer();
	// 线程工作函数 -> 推荐使用
	static void* working(void* arg);
	//// 友元破坏了类的封装
	//friend void* workHard(void* arg);
	
	// 秘钥协商
	string seckeyAgree(RequestMsg* req);
	~ServerOP();

private:
	string getRandKey(KeyLen len);

private:
	string m_serverID;	// 当前服务器的ID
	unsigned short m_port;  //端口号
	map<pthread_t, TcpSocket*> m_list;   //一个线程对应一个客户端的请求操作
	TcpServer* m_server = NULL;  //Tcp通信类
	std::mutex m_mutex;  //用于保护m_list的互斥锁
	std::condition_variable m_condVar;    //条件变量控制线程同步
};

//void* workHard(void* arg);

