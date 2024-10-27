#pragma once
#include<iostream>
#include<string>
class BaseShm
{
public:

	BaseShm(std::string name);  //�����ļ��򿪹����ڴ棨ftok��
	BaseShm(int key);          //����key�򿪹����ڴ�
	BaseShm(std::string name, int size);        //����·�����������ڴ�
	BaseShm(int key, int size);           //����key���������ڴ�

	void* mapShm();     //���̺͹����ڴ潨����ϵ
	int unmapShm();     //���̺͹����ڴ�Ͽ���ϵ
	int delShm();		//ɾ�������ڴ�

	~BaseShm();
private:
	int getShmID(int key, int size, int flag);
	int shmID;
protected:
	void* shmaddr = NULL;
};

