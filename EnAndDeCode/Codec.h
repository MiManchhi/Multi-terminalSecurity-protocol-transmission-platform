#pragma once
#include<iostream>

class Codec
{
public:
	Codec();  //Ĭ�Ϲ���
	virtual std::string encodeMsg(); //����->���л�
	virtual void* decodeMsg(); //����->�����л�
	virtual ~Codec(); //������
};

