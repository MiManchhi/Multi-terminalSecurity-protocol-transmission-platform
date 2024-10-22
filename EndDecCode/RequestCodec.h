#pragma once
#include "Codec.h"
#include "msg.pb.h"
#include<iostream>


//���б�������ݽṹ�壬�򻯲�������
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
	//Ĭ�Ϲ���
	RequestCodec();
	//�˹��캯�����ڽ��� string�����ԭ����
	RequestCodec(std::string encstr);
	//�˹��캯�����ڱ��� ԭ���ݱ�����ַ���
	RequestCodec(RequestInfo* info);
	//��ʼ��Ҫ������ַ������ڿչ���
	void initMessage(std::string encstr);
	//��ʼ��Ҫ�����ԭ�������ڿչ���
	void initMessage(RequestInfo* info);
	//��д����Codec�ı��뺯�������л���  �������л�����ַ���
	std::string encodeMsg(); 
	//��д����Codec�Ľ��뺯���������л���   ���ط����л����ԭ����
	void* decodeMsg();
	//����
	~RequestCodec();
private:
	std::string m_encStr;   //��Ҫ���루�����л������ַ���
	RequestMsg m_msg;   //protobuf���ɵ��࣬�����������л��ͷ����л�
};

