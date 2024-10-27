#include "BaseShm.h"
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

const char RandX = 'x';
BaseShm::BaseShm(int key)
{
	shmID = getShmID(key, 0, 0);
}

BaseShm::BaseShm(std::string name)
{
	int key = ftok(name.data(), RandX);
	shmID = getShmID(key, 0, 0);
}

BaseShm::BaseShm(int key, int size)
{
	shmID = getShmID(key, size, IPC_CREAT | 0664);
}

BaseShm::BaseShm(std::string name, int size)
{
	int key = ftok(name.data(), RandX);
	shmID = getShmID(key, size, IPC_CREAT | 0664);
}

int BaseShm::getShmID(int key, int size, int flag)
{
	std::cout << "share memory size: " << size << std::endl;
	shmID = shmget(key, size, flag);
	if (shmID == -1)
	{
		// Ð´logÈÕÖ¾
		std::cout << "shmget Ê§°Ü" << std::endl;
	}
	return shmID;
}

void* BaseShm::mapShm()
{
	shmaddr = shmat(shmID, NULL, 0);
	if (shmaddr == (void*)-1)
	{
		return NULL;
	}
	return shmaddr;
}

int BaseShm::unmapShm()
{
	int ret = shmdt(shmaddr);
	return ret;
}

int BaseShm::delShm()
{
	int ret = shmctl(shmID, IPC_RMID, NULL);
	return ret;
}

BaseShm::~BaseShm()
{

}