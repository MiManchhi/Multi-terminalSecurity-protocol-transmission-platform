#pragma once
#include"Codec.h"


//�������ģʽ  ��������
class CodecFactory
{
public:
	CodecFactory();
	virtual Codec* createCodec();    //ͨ����̬ʵ�����ɲ�ͬ���͵�Codec
	virtual ~CodecFactory();
};

