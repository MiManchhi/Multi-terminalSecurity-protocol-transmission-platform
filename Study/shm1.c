//#include<stdio.h>
//#include<stdlib.h>
//#include<unistd.h>
//#include<sys/shm.h>
//#include<sys/ipc.h>
//#include<string.h>
//
//int main()
//{
//	//���������ڴ�
//	int shmid = shmget(100, 4096, IPC_CREAT | 0664);
//	//�򿪹����ڴ�
//	shmid = shmget(100, 0, 0);
//	//�����̺ι����ڴ����
//	void* shmaddr = shmat(shmid, NULL, 0);
//	//д���ݵ������ڴ�
//	memcpy(shmaddr, "���ǹ����ڴ� this is sharememory",sizeof("���ǹ����ڴ� this is sharememory"));
//	printf("�����������.....\n");
//	getchar();
//	//���̺͹����ڴ����
//	shmdt(shmaddr);
//	//�رչ����ڴ�
//	shmctl(shmid, IPC_RMID, NULL);
//	return 0;
//}