#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<string.h>

int main()
{

	//�򿪹����ڴ�
	int shmid = shmget(100, 0, 0);
	//�����̺ι����ڴ����
	void* shmaddr = shmat(shmid, NULL, 0);
	//������
	printf("%s\n", (char*)shmaddr);
	//���̺͹����ڴ����
	shmdt(shmaddr);
	//�رչ����ڴ�
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}