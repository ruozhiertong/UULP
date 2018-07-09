/*
读者写者问题。

写者有先。
增加一个互斥量。

(读写锁也能实现 写者有先或读者优先(看系统的读写锁实现)的读者写者问题。)
*/


//写者有先
#include <stdio.h>
#include <pthread.h>

#define MAXMSG	10000
#define READERNUM	4
#define WRITERNUM	4


//写者往里写消息。读者从中读消息。
typedef struct MSG
{
	pthread_mutex_t writer_lock;
	pthread_mutex_t readNum_lock;
	pthread_mutex_t priority_lock;
	char message[MAXMSG];
	int len_msg;
	int reader_num;
}MSG;

MSG g_msg;

void *reader(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&g_msg.priority_lock);
		pthread_mutex_lock(&g_msg.readNum_lock);
		//如果是第一个
		if(g_msg.reader_num == 0)
			pthread_mutex_lock(&g_msg.writer_lock);
		g_msg.reader_num++;
		pthread_mutex_unlock(&g_msg.readNum_lock);
		pthread_mutex_unlock(&g_msg.priority_lock);

		printf("%d reader:%s\n", pthread_self(),g_msg.message);

		sleep(1);

		pthread_mutex_lock(&g_msg.readNum_lock);
		g_msg.reader_num--;
		if(g_msg.reader_num == 0)
		{
			pthread_mutex_unlock(&g_msg.writer_lock);
		}
		pthread_mutex_unlock(&g_msg.readNum_lock);

	}

}

void *writer(void *arg)
{
	char ch = *(char*)arg;
	free(arg);
	printf("%d,%c\n",pthread_self(),ch);

	while(1)
	{
		pthread_mutex_lock(&g_msg.priority_lock);
		pthread_mutex_lock(&g_msg.writer_lock);
		pthread_mutex_unlock(&g_msg.priority_lock);//放在pthread_mutex_unlock(&g_msg.writer_lock);后也行。不影响。

		if(g_msg.len_msg >= MAXMSG-1)
		{
			pthread_mutex_unlock(&g_msg.writer_lock);
			break;
		}
		printf("%d writer:%c\n",pthread_self(), ch);
		g_msg.message[g_msg.len_msg++] = ch;
		sleep(1);
		pthread_mutex_unlock(&g_msg.writer_lock);

	}
}


int main()
{

	pthread_t tid_reader[READERNUM] , tid_writer[WRITERNUM];


	pthread_mutex_init(&g_msg.writer_lock, NULL);
	pthread_mutex_init(&g_msg.readNum_lock, NULL);
	pthread_mutex_init(&g_msg.priority_lock, NULL);

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
	}

	for(int i = 0; i < READERNUM ; i++)
	{
		pthread_join(tid_reader[i], NULL);
	}

	for(int i = 0; i < WRITERNUM; i++)
	{
		pthread_join(tid_writer[i], NULL);
	}


//用init初始化的，要用pthread_mutex_destroy 释放。
	pthread_mutex_destroy(&g_msg.writer_lock);
	pthread_mutex_destroy(&g_msg.readNum_lock);
	pthread_mutex_destroy(&g_msg.priority_lock);


	return 0;
}