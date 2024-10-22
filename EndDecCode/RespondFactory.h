#pragma once
#include "CodecFactory.h"
#include "Codec.h"
#include "RespondCodec.h"
class RespondFactory :
    public CodecFactory
{
public:
    RespondFactory(std::string encStr);
    RespondFactory(RespondInfo* info);
    Codec* createCodec();
    ~RespondFactory();
private:
    bool m_flag;
    std::string m_encStr;
    RespondInfo* m_info;
};

