#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "ServerOP.h"
#include "TcpSocket.h"
#include "RequestFactory.h"
#include "RequestCodec.h"
#include "RespondCodec.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include "json/json.h"
#include "Hash.h"
using namespace std;
using namespace Json;

ServerOP::ServerOP(string json)
{
	//解析json文件 ，读文件->Value
	ifstream ifs(json);
	if (!ifs.is_open()) {
		std::cerr << "无法打开配置文件: " << json << std::endl;
		return;
	}
	//std::string fileContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	//std::cout << "读取的配置文件内容: " << endl << fileContent << std::endl;
	Reader r;
	Value root;
	r.parse(ifs, root);
	if (!r.parse(ifs, root)) {
		std::cerr << "解析 JSON 失败: " << r.getFormattedErrorMessages() << std::endl;
		return;
	}
	//将root中的键值对value取出
	m_port = root["Port"].asInt();
	cout << m_port << endl;
	m_serverID = root["ServerID"].asString();
	cout << m_serverID << endl;
}

void ServerOP::startServer()
{
	//创建socket
	m_server = new TcpServer;
	//设置监听
	m_server->setListen(m_port);
	//进入循环等待客户端请求
	while (true)
	{
		cout << "等待客户端连接" << endl;
		TcpSocket* tcp = m_server->acceptConn();  //返回用于通信的tcp
		if (tcp == NULL)
		{
			continue;
		}
		cout << "与客户端连接成功" << endl;
		//通信
		pthread_t tid;
		//回调函数可以是类的静态函数，类的友元函数（不推荐）,普通函数（不推荐）
		pthread_create(&tid, NULL, working, this);
		m_list.insert(make_pair(tid, tcp));
	}
}

void* ServerOP::working(void* arg)
{
	//通过参数传递this对象，进行转换
	ServerOP* op = (ServerOP*)arg;
	//从opServerOP对象中取出套接字对象
	TcpSocket* tcp = op->m_list[pthread_self()];  //pthread_self返回当前线程的线程ID
	//接收客户端数据
	string msg = tcp->recvMsg();
	//反序列化
	CodecFactory* factory = new RequestFactory(msg);
	Codec* c = factory->createCodec();
	RequestMsg* req = (RequestMsg*)c->decodeMsg();
	//取数据判断客户端请求
	switch (req->cmdtype())
	{
	case 1:
		//密钥协商
		op->seckeyAgree(req);
		break;
	case 2:
		//密钥校验
		break;
	case 3:
		//密钥注销
		break;
	default:
		break;
	}
	//释放资源
	delete factory;
	delete c;
	//tcp对象处理
	return NULL;
}

string ServerOP::seckeyAgree(RequestMsg* reqMsg)
{
	return string();
}

ServerOP::~ServerOP()
{
}

string ServerOP::getRandKey(KeyLen len)
{
	return string();
}
