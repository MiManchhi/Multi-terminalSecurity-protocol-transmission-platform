#include <iostream>
#include "RequestCodec.h"
#include "RespondCodec.h"
#include "Codec.h"
#include "RequestFactory.h"
#include "RespondFactory.h"

using namespace std;

int main()
{
	// 数据编码
	RequestInfo info{ 1, "client", "server", "x00911", "hello, world" };
	//RequestCodec req(&info);
	//std::string enstr = req.encodeMsg();
	CodecFactory* factory = new RequestFactory(&info);
	Codec* codec = factory->createCodec();
	std::string str = codec->encodeMsg();
	cout << "序列化数据: " << str << endl;
	delete factory;
	delete codec;

	// 数据解码
	//RequestCodec req1(enstr);
	//RequestMsg *msg = (RequestMsg*)req1.decodeMsg();
	//cout << msg->clientid() << endl
	//	<< msg->cmdtype() << endl
	//	<< msg->data() << endl
	//	<< msg->serverid() << endl
	//	<< msg->sign() << endl;
	factory = new RequestFactory(str);
	codec = factory->createCodec();
	RequestMsg* r1 = (RequestMsg*)codec->decodeMsg();
	cout << "cmdtype: " << r1->cmdtype()
		<< ", clinetID: " << r1->clientid()
		<< ", serverID: " << r1->serverid()
		<< ", sign: " << r1->sign()
		<< ", data: " << r1->data() << endl;
	delete factory;
	delete codec;

	cout << endl << "=========================" << endl;

	RespondInfo resinfo{ 1, 999, "luffy", "zoro", "change world 666 !" };
	factory = new RespondFactory(&resinfo);
	codec = factory->createCodec();
	str = codec->encodeMsg();
	delete factory;
	delete codec;

	factory = new RespondFactory(str);
	codec = factory->createCodec();
	RespondMsg* r2 = (RespondMsg*)codec->decodeMsg();
	cout << "status: " << r2->status()
		<< ", seckeyID: " << r2->seckeyid()
		<< ", clinetID: " << r2->clientid()
		<< ", serverID: " << r2->serverid()
		<< ", data: " << r2->data() << endl;

	delete factory;
	delete codec;

	return 0;
}