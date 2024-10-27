#pragma once
#include<iostream>
#include<string>
class BaseShm
{
public:

	BaseShm(std::string name);  //按照文件打开共享内存（ftok）
	BaseShm(int key);          //按照key打开共享内存
	BaseShm(std::string name, int size);        //按照路径创建共享内存
	BaseShm(int key, int size);           //按照key创建共享内存

	void* mapShm();     //进程和共享内存建立关系
	int unmapShm();     //进程和共享内存断开关系
	int delShm();		//删除共享内存

	~BaseShm();
private:
	int getShmID(int key, int size, int flag);
	int shmID;
protected:
	void* shmaddr = NULL;
};

