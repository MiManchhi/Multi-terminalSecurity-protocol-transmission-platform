#include "RequestCodec.h"

RequestCodec::RequestCodec()
{
}

RequestCodec::RequestCodec(std::string encstr)
{
	initMessage(encstr);
}

RequestCodec::RequestCodec(RequestInfo* info)
{
	initMessage(info);
}

void RequestCodec::initMessage(std::string encstr)
{
	m_encStr = encstr;
}

void RequestCodec::initMessage(RequestInfo* info)
{
	m_msg.set_cmdtype(info->cmd);
	m_msg.set_clientid(info->clientID);
	m_msg.set_serverid(info->serverID);
	m_msg.set_sign(info->sign);
	m_msg.set_data(info->data);
}

std::string RequestCodec::encodeMsg()
{
	std::string output;
	m_msg.SerializeToString(&output);  //将序列化后的数据存放在字符串中
	return output;
	
}

void* RequestCodec::decodeMsg()
{
	m_msg.ParseFromString(m_encStr); 
	return &m_msg;
}

RequestCodec::~RequestCodec()
{

}