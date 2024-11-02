#include "RespondFactory.h"

RespondFactory::RespondFactory(std::string enc)
{
	m_flag = false;
	m_encStr = enc;
}
RespondFactory::RespondFactory(RespondInfo* info)
{
	m_flag = true;
	m_info = info;
}

Codec* RespondFactory::createCodec()
{
	Codec* c = NULL;
	if (m_flag)
	{
		c = new RespondCodec(m_info);
	}
	else
	{
		c = new RespondCodec(m_encStr);
	}
	return c;
}

RespondFactory::~RespondFactory()
{

}