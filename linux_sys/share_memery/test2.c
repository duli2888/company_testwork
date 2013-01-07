//共享内存区段的挂载,脱离和使用
//理解共享内存区段就是一块大内存
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#define MY_SHM_ID 67483
int main(  )
{
	//共享内存区段的挂载和脱离
	int shmid,ret;
	void* mem;
	shmid=shmget( MY_SHM_ID,0,0 );
	if( shmid>=0 )
	{
		mem=shmat( shmid,( const void* )0,0 );
		//shmat()返回进程地址空间中指向区段的指针
		if( ( int )mem!=-1 )
		{
			printf( "Shared memory was attached in our address space at %p/n",mem );
			//向共享区段内存写入数据
			strcpy( ( char* )mem,"This is a test string./n" );
			printf( "%s/n",(char*)mem );
			//脱离共享内存区段
			ret=shmdt( mem );
			if( ret==0 )
				printf( "Successfully detached memory /n" );
			else
				printf( "Memory detached failed %d/n",errno );
		}
		else
			printf( "shmat(  ) failed/n" );

	}
	else
		printf( "shared memory segment not found/n" );
	return 0;
}

