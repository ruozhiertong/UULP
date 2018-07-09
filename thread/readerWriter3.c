/*
读者写者问题。
用读写锁实现。

读写锁
适合读次数更多的情况。
不同实现可能不同的情况。有的读者优先，有的写者优先。
读写锁如果是一种读者优先。
读写锁如果是一种写者有先。

mac中(posix)读写锁是一种写者优先。 有的说linux中是读者优先：http://www.360doc.com/content/15/0308/13/7377734_453517429.shtml。
用读写锁实现 写者有先的读者写者问题
*/


//这里是mac posix。读写锁是写者有先。
#include <stdio.h>
#include <pthread.h>

#define MAXMSG	10000
#define READERNUM	4
#define WRITERNUM	4


//写者往里写消息。读者从中读消息。
typedef struct MSG
{
	pthread_rwlock_t rwlock;

	char message[MAXMSG];
	int len_msg;
	//int reader_num;
}MSG;

MSG g_msg;
void *reader(void *arg)
{
	while(1)
	{
		pthread_rwlock_rdlock(&g_msg.rwlock);
		
		printf("%d reader:%s\n", pthread_self(),g_msg.message);

		sleep(1);

		pthread_rwlock_unlock(&g_msg.rwlock);

	}

}

void *writer(void *arg)
{
	char ch = *(char*)arg;
	free(arg);
	printf("%d,%c\n",pthread_self(),ch);

	while(1)
	{
		pthread_rwlock_wrlock(&g_msg.rwlock);
		if(g_msg.len_msg >= MAXMSG-1)
		{
			pthread_rwlock_unlock(&g_msg.rwlock);
			break;
		}
		printf("%d writer:%c\n",pthread_self(), ch);
		g_msg.message[g_msg.len_msg++] = ch;
		sleep(1);
		pthread_rwlock_unlock(&g_msg.rwlock);

	}
}


int main()
{

	pthread_t tid_reader[READERNUM] , tid_writer[WRITERNUM];

	pthread_rwlock_init(&g_msg.rwlock ,NULL);

	for(int i = 0; i < READERNUM ; i++)
	{
		pthread_create(&tid_reader[i],NULL, reader, NULL); //函数指针可以直接函数名传递，也可以函数名取地址。
	}

	for(int i = 0; i < WRITERNUM; i++)
	{
		char *ch = (char*)malloc(1); //如果这里没有malloc，而是用一个局部变量，而多次pthread_create用到的地址，多次对ch赋值，可能会影响其他线程中的值，因为可能局部变量都被系统分配了同一个地址，导致值变化。 
		*ch = '1' + i; 
		printf("in main %c\n", *ch);
		pthread_create(&tid_writer[i], NULL, &writer, (void*)ch);
		printf("tid_writer:%d\n", tid_writer[i]); //tid_ 的值和线程中运行的pthread_self() 一样。
	}

	for(int i = 0; i < READERNUM ; i++)
	{
		pthread_join(tid_reader[i], NULL);
	}

	for(int i = 0; i < WRITERNUM; i++)
	{
		pthread_join(tid_writer[i], NULL);
	}
	
	pthread_rwlock_destroy(&g_msg.rwlock);

	return 0;
}