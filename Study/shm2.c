#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<string.h>

int main()
{

	//打开共享内存
	int shmid = shmget(100, 0, 0);
	//将进程何共享内存关联
	void* shmaddr = shmat(shmid, NULL, 0);
	//读数据
	printf("%s\n", (char*)shmaddr);
	//进程和共享内存分离
	shmdt(shmaddr);
	//关闭共享内存
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}