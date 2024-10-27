//#include<stdio.h>
//#include<stdlib.h>
//#include<unistd.h>
//#include<sys/shm.h>
//#include<sys/ipc.h>
//#include<string.h>
//
//int main()
//{
//	//创建共享内存
//	int shmid = shmget(100, 4096, IPC_CREAT | 0664);
//	//打开共享内存
//	shmid = shmget(100, 0, 0);
//	//将进程何共享内存关联
//	void* shmaddr = shmat(shmid, NULL, 0);
//	//写数据到共享内存
//	memcpy(shmaddr, "这是共享内存 this is sharememory",sizeof("这是共享内存 this is sharememory"));
//	printf("按任意键继续.....\n");
//	getchar();
//	//进程和共享内存分离
//	shmdt(shmaddr);
//	//关闭共享内存
//	shmctl(shmid, IPC_RMID, NULL);
//	return 0;
//}