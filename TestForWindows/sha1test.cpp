#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<openssl/sha.h>
#include<openssl/rsa.h>
#include<openssl/pem.h>
#include<vector>

using namespace std;
extern "C"
{
#include <openssl/applink.c>
};

using namespace std;

void sha1test()
{
	//初始化
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	//添加数据
	SHA1_Update(&ctx, "hello", sizeof("hello"));
	SHA1_Update(&ctx, " world", sizeof(" world"));
	//哈希运算
	unsigned char* md = new unsigned char[SHA_DIGEST_LENGTH];
	SHA1_Final(md, &ctx);
	//转换为16进制
	char* res = new char[SHA_DIGEST_LENGTH * 2 + 1];
	for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&res[i * 2], "%02x", md[i]);
	}
	cout << "sha1:" << res << endl;
}

void rsaTest()
{
	//创建rsa变量
	RSA* rsa = RSA_new();
	//创建bignum对象并初始化
	BIGNUM* e = BN_new();
	BN_set_word(e, 12345);
	//生成密钥对
	RSA_generate_key_ex(rsa, 1024, e, NULL);
	//将密钥对写入到磁盘
	FILE* fp = fopen("public.pem", "w");
	PEM_write_RSAPublicKey(fp, rsa);
	fclose(fp);
	//写私钥
	fp = fopen("public.pem", "w");
	PEM_write_RSAPrivateKey(fp, rsa,NULL,NULL,0,NULL,NULL);
	fclose(fp);
}

void test()
{
	vector<char> cs = { '~','!','@','#','$','%','^','&','*','(',')',
'_','+','{','}','|','\\','\'',';','[',']' };
	for (auto val : cs)
	{
		cout << val << endl;
	}
}

int main()
{
	test();
	//rsaTest();
	//sha1test();
	return 0;
}