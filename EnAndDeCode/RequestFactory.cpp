#include "RequestFactory.h"

RequestFactory::RequestFactory(std::string enc)
{
	m_flag = false;
	m_encStr = enc;
}

RequestFactory::RequestFactory(RequestInfo* info)
{
	m_flag = true;
	m_info = info;
}

Codec* RequestFactory::createCodec()
{
	Codec* c = NULL;
	if (m_flag)
	{
		c = new RequestCodec(m_info);
	}
	else
	{
		c = new RequestCodec(m_encStr);
	}
	return c;
}

RequestFactory::~RequestFactory()
{

}
