#pragma once
#include "Codec.h"
#include "msg.pb.h"
#include<iostream>

//进行编码的数据结构体，简化参数输入
struct RespondInfo
{
	int status;
	int seckeyID;
	std::string clientID;
	std::string serverID;
	std::string data;
};

class RespondCodec :
    public Codec
{
public:
	//默认构造
	RespondCodec();
	//此构造函数用来初始化解码的字符串
	RespondCodec(std::string enc);
	//此构造函数用于初始化编码的原数据
	RespondCodec(RespondInfo* info);
	//用于初始化用于解码的字符串
	void initMessage(std::string enc);
	//初始化用于编码的原数据
	void initMessage(RespondInfo* info);
	//重写Codec父类的编码函数
	std::string encodeMsg();
	//重写Codec父类的解码函数
	void* decodeMsg();
	//析构
	~RespondCodec();

private:
	std::string m_encstr; //需要进行解码的字符串
	RespondMsg m_msg;  //需要进行编码的原数据
};

