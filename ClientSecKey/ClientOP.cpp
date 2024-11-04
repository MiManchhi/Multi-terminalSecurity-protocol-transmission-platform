#include<fstream>
#include<sstream>
#include<iostream>
#include"json/json.h"
#include "ClientOP.h"
#include"RequestCodec.h"
#include"RequestFactory.h"
#include"RsaCrypto.h"
#include"TcpSocket.h"
#include"RespondCodec.h"
#include"RespondFactory.h"
#include"Hash.h"

using namespace std;
using namespace Json;   //jsoncpp的命名空间

ClientOP::ClientOP(string jsonFile)
{
	//解析json文件  读文件->Value
	ifstream ifs(jsonFile);
	Reader r;
	Value root;
	r.parse(ifs, root);
	//将root中的键值对vale值取出
	m_info.ServerID = root["ServerID"].asString();
	m_info.ClientID = root["ClientID"].asString();
	m_info.port = root["Port"].asInt();
	m_info.ip = root["ServerIP"].asString();
}

ClientOP::~ClientOP()
{

}

/*
客户端生成密钥对，将公钥（数据）进行签名并经过哈希运算后发送给服务器
服务器使用得到的公钥将对称加密的密钥（数据）加密后发送给客户端
客户端使用私钥将对称加密的密钥解密拿到对称加密的密钥
*/
bool ClientOP::seckeyAgree()
{
	cout << "这是密钥协商" << endl;
	//生成密钥对
	Cryptographic rsa;
	rsa.generateRsakey(1024);
	//读公钥文件
	ifstream ifs("public.pem");
	stringstream str;
	str << ifs.rdbuf();
	//创建哈希对象
	Hash sha1(T_SHA1);
	sha1.addData(str.str());
	//初始化序列化数据  序列化的类对象->工厂类创建
	RequestInfo reqInfo;  //这个结构体用来初始化RequestCodec类，初始化类成员RequestMsg（由protoc生成）
	reqInfo.clientID = m_info.ClientID;
	reqInfo.cmd = 1;   //密钥协商
	reqInfo.data = str.str();  //非对称加密的公钥
	reqInfo.serverID = m_info.ServerID;
	reqInfo.sign = rsa.rsaSign(sha1.result());   //公钥的哈希值签名
	cout << "签名完成" << endl;

	CodecFactory* factory = new RequestFactory(&reqInfo);
	Codec* c = factory->createCodec();
	//得到序列化后的数据
	string encstr = c->encodeMsg();
	//释放资源
	delete factory;
	delete c;
	//套接字通信  连接服务器
	TcpSocket* tcp = new TcpSocket;
	int ret = tcp->connectServer(m_info.ip, m_info.port);
	if (ret != 0)
	{
		cout << "连接服务器失败" << endl;
		return false;
	}
	cout << "连接服务器成功" << endl;
	//发送序列化后的数据
	tcp->sendMsg(encstr);
	//等待服务器回复
	string msg = tcp->recvMsg();
	//解析服务器数据
	factory = new RespondFactory(msg);
	c = factory->createCodec();
	//数据还原到RespondMsg
	RespondMsg* resData = (RespondMsg*)c->decodeMsg();
	if (!resData->status())
	{
		cout << "密钥协商失败" << endl;
		return false;
	}
	//将得到的密文解密
	string key = rsa.rsaPriKeyDecrypt(resData->data());
	cout << "对称加密的密钥:" << key << endl;
	//释放资源
	delete factory;
	delete c;
	//断开连接
	tcp->closeConnect();
	delete tcp;
	return true;
}

void ClientOP::seckeyCheck()
{
	cout << "这是密钥校验" << endl;

}

void ClientOP::seckeyCancel()
{
	cout << "这是密钥注销" << endl;
}