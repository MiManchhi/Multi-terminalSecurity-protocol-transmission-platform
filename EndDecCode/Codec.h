#pragma once
#include<iostream>

class Codec
{
public:
	Codec();  //默认构造
	virtual std::string encodeMsg(); //编码->序列化
	virtual void* decodeMsg(); //解码->反序列化
	virtual ~Codec(); //虚析构
};

