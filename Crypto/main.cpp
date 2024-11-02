#include <iostream>
#include "Hash.h"
#include <string>
#include "RsaCrypto.h"
#include "AesCrypto.h"
using namespace std;


void hashTest()
{
	Hash md5(HashType::T_MD5);
	md5.addData("hello");
	md5.addData(", ");
	md5.addData("world");
	string rs = md5.result();
	cout << "md5: " << rs << endl;

	Hash sha1(HashType::T_SHA1);
	sha1.addData("hello, world");
	rs = sha1.result();
	cout << "sha1: " << rs << endl;

	Hash sha224(HashType::T_SHA224);
	sha224.addData("hello, world");
	rs = sha224.result();
	cout << "sha224: " << rs << endl;

	Hash sha256(HashType::T_SHA256);
	sha256.addData("hello, world");
	rs = sha256.result();
	cout << "sha256: " << rs << endl;

	Hash sha384(HashType::T_SHA384);
	sha384.addData("hello, world");
	rs = sha384.result();
	cout << "sha384: " << rs << endl;

	Hash sha512(HashType::T_SHA512);
	sha512.addData("hello, world");
	rs = sha512.result();
	cout << "sha512: " << rs << endl;
}

int main()
{
	string text = "ע������A�Ĺ��캯�����ڲ����б��ͳ�ʼ���б�ǰ������try�ؼ��֣�Ȼ���캯���ͱ��ָ�Ϊ�������֣�ǰ���ǳ�ʼ���������ǳ�ʼ��ʱ�Ĵ�������Ҫָ�����ǣ�catch�����沶�񵽵��쳣���ܱ����ԣ���catch���б�����һ��throw��������׳��쳣�����û�У���Ĭ�ϻὫԭ�����񵽵��쳣�����׳������һ�����Ϊ�ǲ�ͬ�ġ���������������п��Է���A�Ὣ���񵽵��쳣ԭ�ⲻ���׳�";
	string key = "12345678abcdefgh";
	try {
		AesCrypto aes(key);
		cout << "Դ���� :" << text << endl;
		string ret = aes.aesCBCEncrypt(text);
		ret = aes.aesCBCDecrypt(ret);
		cout << "��������: " << ret << endl;
	}
	catch (exception e)
	{
		cout << "exception: " << e.what() << endl;
	}

#if 1
	hashTest();
	Cryptographic crypto;
	crypto.generateRsakey(2048);

	string str = "�ñ�̸ı����� - good good study, day day up!!!";
	cout << "ԭʼ����: " << str << endl;
	str = crypto.rsaPubKeyEncrypt(str);
	cout << "��������: " << str << endl;
	str = crypto.rsaPriKeyDecrypt(str);
	cout << "��������: " << str << endl;

	string sign = crypto.rsaSign(str);
	bool bl = crypto.rsaVerify(str, sign);
	cout << "verify: " << bl << endl;
#endif
	return 0;
}