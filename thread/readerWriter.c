/*
读者写者问题：
一般而言，对一个数据集，为了保证数据的完整 性、正确性，允许多个读者进程同时访问，
但是不允许一个写者进程同其它任何一个进程（读者 或者写者）同时访问，而这类问题就称之为"读者-写者"问题。


单个读者单个写者。
单个读者多个写者。
多个读者单个写者。
多个读者多个写者。

读者有先。
可能造成写者饥饿。

写者有先。
*/


//读者有先
#include <stdio.h>
#include <pthread.h>

#define MAXMSG	10000
#define READERNUM	4
#define WRITERNUM	4


//写者往里写消息。读者从中读消息。

//还可以改进，试试条件变量。
typedef struct MSG
{
	pthread_mutex_t writer_lock;
	pthread_cond_t	empty_cond; //条件变量
	pthread_cond_t	full_cond;
	pthread_mutex_t readNum_lock;
	char message[MAXMSG];
	int len_msg;
	int reader_num;
}MSG;

MSG g_msg ={PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER ,0,0,0};

void *reader(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&g_msg.readNum_lock);
		//如果是第一个
		if(g_msg.reader_num == 0)
			pthread_mutex_lock(&g_msg.writer_lock);
		g_msg.reader_num++;
		pthread_mutex_unlock(&g_msg.readNum_lock);

		//if(g_msg.len_msg == 0) 并不做消费操作，只是读取，因此不需要做判断。


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
		pthread_mutex_lock(&g_msg.writer_lock);
		if(g_msg.len_msg >= MAXMSG-1)//缓冲区满
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


//用条件变量改进。不做消费，因此也就没有使用条件变量。
void *reader2(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&g_msg.readNum_lock);
		//如果是第一个
		if(g_msg.reader_num == 0)
			pthread_mutex_lock(&g_msg.writer_lock);
		g_msg.reader_num++;
		pthread_mutex_unlock(&g_msg.readNum_lock);

		//if(g_msg.len_msg == 0) 并不做消费操作，只是读取，因此不需要做判断。

		/*
		//如果做消费。
		while(g_msg.len_msg == 0)
		{
			printf("g_msg is null ,reader2 wait...");
			pthread_cond_wait(&g_msg.empty_cond,&g_msg.writer_lock);
		}

		*/


		printf("%d reader:%s\n", pthread_self(),g_msg.message);

		sleep(1);


		/*
		
		pthread_cond_signal(&g_msg.full_cond);
		
		*/
		pthread_mutex_lock(&g_msg.readNum_lock);
		g_msg.reader_num--;
		if(g_msg.reader_num == 0) 
		{
			pthread_mutex_unlock(&g_msg.writer_lock);
		}
		pthread_mutex_unlock(&g_msg.readNum_lock);

	}

}

//用条件变量改进。不做消费，因此也就没有使用条件变量。
void *writer2(void *arg)
{
	char ch = *(char*)arg;
	free(arg);
	printf("%d,%c\n",pthread_self(),ch);

	while(1)
	{
		pthread_mutex_lock(&g_msg.writer_lock);
		/*
		while(g_msg.len_msg >= MAXMSG-1)
		{
			//TODO:有没有可能在writer线程signal被writer线程接收到，又进入wait，那么这个signal信号就没了，而此时reader线程还在等待信号。如果这样虚假唤醒导致全部线程都在wait，那怎么处理？？
			//有可能的。
			printf("g_msg is full, writer2 wait...");
			pthread_cond_wait(&g_msg.full_cond, &g_msg.writer_lock);
		}
		*/

		//因为这里读者写者并没有做消费的操作的，因此如果缓冲区满，就直接退出。不需要条件变量来做什么了。
		if(g_msg.len_msg >= MAXMSG-1)//缓冲区满
		{
			pthread_mutex_unlock(&g_msg.writer_lock);
			break;
		}
		printf("%d writer:%c\n",pthread_self(), ch);
		g_msg.message[g_msg.len_msg++] = ch;
		sleep(1);

		/*

		pthread_cond_signal(&g_msg.empty_cond);
		
		*/
		pthread_mutex_unlock(&g_msg.writer_lock);
	}
}


int main()
{

	pthread_t tid_reader[READERNUM] , tid_writer[WRITERNUM];

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

	return 0;
}