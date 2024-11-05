#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <mutex>  //互斥锁
#include <condition_variable>   //条件变量库
#include <vector>
#include <ctime>
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
	/*r.parse(ifs, root);*/
	if (!r.parse(ifs, root)) {
		std::cerr << "解析 JSON 失败: " << r.getFormattedErrorMessages() << std::endl;
		return;
	}
	//将root中的键值对value取出
	m_port = root["Port"].asInt();
	cout << "m_port:" << m_port << endl;
	m_serverID = root["ServerID"].asString();
	cout << "m_serverID:" << m_serverID << endl;
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
		//使用互斥锁保护m_list的插入操作
		/*
			使用互斥锁保护m_list的插入操作
			lock_guard<mutex> lock(m_mutex)
				构造时自动加锁，析构时自动解锁。
			unique_lock<mutex> lock()
				支持构造时不加锁、随时加锁、解锁等操作，通过方法 lock() 和 unlock() 控制锁的状态。
				可以用于条件变量等待中，因为它可以释放和重新获取锁。
				离开作用域时也自动解锁
		*/
		pthread_create(&tid, NULL, working, this);
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_list.insert(make_pair(tid, tcp));
		}
		//通知子线程
		/*
			子线程可能比主线程先执行，使用条件变量来同步线程
			当主线程插入数据后再通知子线程工作
			notify_one
				通知一个等待该条件变量的线程继续执行。
				如果有多个线程在条件变量上等待，notify_one 会随机唤醒其中一个。
			notify_all
				通知所有等待该条件变量的线程继续执行。
			wait
				用于阻塞当前线程，直到收到条件变量的通知或者条件满足。
				m_condVar.wait(lock, [op, selfTid] {
				return op->m_list.find(selfTid) != op->m_list.end();
				});
				当wait处于阻塞状态时会释放锁，避免死锁，当条件满足时或者被唤醒时加锁

			使用条件变量的步骤
			创建一个条件变量对象（std::condition_variable）和一个互斥锁（std::mutex）。
			等待线程调用 wait 方法，在条件满足前进入等待状态。
			通知线程调用 notify_one 或 notify_all 方法，唤醒等待的线程。
		*/
		m_condVar.notify_all();
	}
}

void* ServerOP::working(void* arg)
{
	//通过参数传递this对象，进行转换
	ServerOP* op = (ServerOP*)arg;
	//从opServerOP对象中取出套接字对象
	pthread_t selfTid = pthread_self();
	TcpSocket* tcp;
	{
		//通过锁保护访问m_list
		std::unique_lock<std::mutex> lock(op->m_mutex);  //加锁    出作用域后自动解锁
		op->m_condVar.wait(lock, [op, selfTid] {
			return op->m_list.find(selfTid) != op->m_list.end();
			});
		tcp = op->m_list[pthread_self()];   //pthread_self返回当前线程的线程ID
	}
	//接收客户端数据
	string msg = tcp->recvMsg();
	//反序列化
	CodecFactory* factory = new RequestFactory(msg);
	Codec* c = factory->createCodec();
	RequestMsg* req = (RequestMsg*)c->decodeMsg();
	string data = string();
	//取数据判断客户端请求
	switch (req->cmdtype())
	{
	case 1:
		//密钥协商
		data = op->seckeyAgree(req);
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
	//tcp对象处理  发送数据
	tcp->sendMsg(data);
	tcp->closeConnect();
	{
		std::lock_guard<std::mutex> lock(op->m_mutex);
		op->m_list.erase(pthread_self());
	}
	delete tcp;
	return NULL;
}

string ServerOP::seckeyAgree(RequestMsg* req)
{
	//对签名进行校验 -> 公钥解密 -> 得到公钥
	//将收到的公钥写入磁盘
	ofstream ofs("public.pem");
	ofs << req->data();
	ofs.close();
	//创建非对称加密对象
	RespondInfo info;
	Hash sha(T_SHA1);
	sha.addData(req->data());
	Cryptographic rsa("public.pem", false);
	bool verifyRet = rsa.rsaVerify(sha.result(), req->sign());
	if (verifyRet == false)
	{
		cout << "签名校验失败" << endl;
		info.status = false;
	}
	else
	{
		cout << "签名校验成功" << endl;
		//生成随机字符串
		string key = getRandKey(Len16);
		cout << "生成的随机字符串" << key << endl;
		//通过公钥加密
		string seckey = rsa.rsaPubKeyEncrypt(key);
		cout << "加密后的数据" << seckey << endl;
		//初始化回复的数据
		info.clientID = req->clientid();
		info.data = seckey;
		info.seckeyID = 12;  //需要数据库操作
		info.serverID = m_serverID;
		info.status = true;
	}
	//序列化
	CodecFactory* fac = new RespondFactory(&info);
	Codec* c = fac->createCodec();
	string encMsg = c->encodeMsg();
	//发送数据
	return encMsg;
}

ServerOP::~ServerOP()
{
	if (m_server != NULL)
	{
		delete m_server;
		m_server = NULL;
	}
}

// 要求: 字符串中包含: a-z, A-Z, 0-9, 特殊字符
string ServerOP::getRandKey(KeyLen len)
{
	int flag = 0;
	srand((unsigned)time(0));
	string randStr = string();
	vector<char> cs = { '~','!','@','#','$','%','^','&','*','(',')',
	'_','+','{','}','|','\\','\'',';','[',']' };
	for (int i = 0; i < len; i++)
	{
		flag = rand() % 4;
		switch (flag)
		{
		case 0:  // a-z
			randStr.append(1, 'a' + rand() % 26);
			break;
		case 1:  //A-Z
			randStr.append(1, 'A' + rand() % 26);
			break;
		case 2:  //0-9
			randStr.append(1, '0' + rand() % 10);
			break;
		case 3:  //特殊字符
			randStr.append(1, cs[rand() % cs.size()]);
			break;
		default:
			break;
		}
	}
	return randStr;
}
