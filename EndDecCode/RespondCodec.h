#pragma once
#include "Codec.h"
#include "msg.pb.h"
#include<iostream>

//���б�������ݽṹ�壬�򻯲�������
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
	//Ĭ�Ϲ���
	RespondCodec();
	//�˹��캯��������ʼ��������ַ���
	RespondCodec(std::string enc);
	//�˹��캯�����ڳ�ʼ�������ԭ����
	RespondCodec(RespondInfo* info);
	//���ڳ�ʼ�����ڽ�����ַ���
	void initMessage(std::string enc);
	//��ʼ�����ڱ����ԭ����
	void initMessage(RespondInfo* info);
	//��дCodec����ı��뺯��
	std::string encodeMsg();
	//��дCodec����Ľ��뺯��
	void* decodeMsg();
	//����
	~RespondCodec();

private:
	std::string m_encstr; //��Ҫ���н�����ַ���
	RespondMsg m_msg;  //��Ҫ���б����ԭ����
};

