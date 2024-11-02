#pragma once
#include "Codec.h"
#include "msg.pb.h"
#include<iostream>


//进行编码的数据结构体，简化参数输入
struct RequestInfo
{
	int cmd;
	std::string clientID;
	std::string serverID;
	std::string sign;
	std::string data;
};

class RequestCodec :
    public Codec
{
public:
	//默认构造
	RequestCodec();
	//此构造函数用于解码 string解码成原数据
	RequestCodec(std::string encstr);
	//此构造函数用于编码 原数据编码成字符串
	RequestCodec(RequestInfo* info);
	//初始化要解码的字符串用于空构造
	void initMessage(std::string encstr);
	//初始化要编码的原数据用于空构造
	void initMessage(RequestInfo* info);
	//重写父类Codec的编码函数（序列化）  返回序列化后的字符串
	std::string encodeMsg(); 
	//重写父类Codec的解码函数（反序列化）   返回反序列化后的原数据
	void* decodeMsg();
	//析构
	~RequestCodec();
private:
	std::string m_encStr;   //需要解码（反序列化）的字符串
	RequestMsg m_msg;   //protobuf生成的类，用来进行序列化和反序列化
};

