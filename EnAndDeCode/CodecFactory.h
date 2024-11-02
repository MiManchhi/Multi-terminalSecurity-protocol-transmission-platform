#pragma once
#include"Codec.h"


//工厂设计模式  工厂基类
class CodecFactory
{
public:
	CodecFactory();
	virtual Codec* createCodec();    //通过多态实现生成不同类型的Codec
	virtual ~CodecFactory();
};

