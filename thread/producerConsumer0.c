/*
多线程的生产者/消费者模型。
按照生产者消费者数量：
单个生产者，单个消费者。
单个生产者，多个消费者。
多个生产者，单个消费者。
多个生产者，多个消费者。
按照共享资源：
有限缓冲区。
无限缓冲区。

一般常见的，有限缓冲区，多个生产者，多个消费者。

代码讨论有限缓冲区，多个生产者，多个消费者。

==>衍生出各种变体：读者写者问题。等。
*/


/*
producerConsumer0.c
不带条件变量。会导致CPU空转，浪费。


gcc producerConsumer0.c -lpthread
*/

#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define FOODNUMMAX	5
#define CONSUMERNUM	1
#define PRODUCERNUM	3	

typedef struct food
{
	char food_name[81]; //food名称
	int  food_calorie;  //卡路里
}FOOD;

typedef struct food_buf
{
	pthread_mutex_t mutex;
	int food_num;
	FOOD food_arr[FOODNUMMAX];
}FOOD_BUF; //缓冲区

FOOD_BUF food_buf;
//food_buf.mutex = PTHREAD_MUTEX_INITIALIZER;

void * produce_food(void * arg)
{
	FOOD product = *(FOOD*)arg;
	while(1)
	{
		pthread_mutex_lock(&food_buf.mutex);

		if(food_buf.food_num < FOODNUMMAX)
		{
			//produce
			printf("%ld producer is producing %d %s , its calorie is %d\n",pthread_self(),food_buf.food_num + 1 , product.food_name, product.food_calorie);
			food_buf.food_arr[food_buf.food_num].food_calorie = product.food_calorie;
			strcpy(food_buf.food_arr[food_buf.food_num].food_name, product.food_name);
			food_buf.food_num++;
		}
		else
		{
			//当不满足条件时，什么事也没做。而且继续一直执行，下次还有可能不满足条件，还是什么事没做。
			//可能这样一直持续，导致CPU空转，浪费。
			printf("%ld food is full. produce nothing. To wait\n", pthread_self());
		}

		pthread_mutex_unlock(&food_buf.mutex);

		//produce
		sleep(1);

	}
}


void *consume_food(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&food_buf.mutex);
		if(food_buf.food_num > 0)
		{
			//consume
			printf("%ld consumer is consuming %d %s , its calorie is %d\n",pthread_self(), food_buf.food_num, food_buf.food_arr[food_buf.food_num - 1].food_name, food_buf.food_arr[food_buf.food_num - 1].food_calorie);
			food_buf.food_num--;
		}
		else
		{
			//没有使用条件变量，这里会一直跑完，下次还可能会在这里跑。CPU空转，浪费。
			//使用条件变量的话，要加while，避免虚假唤醒。
			printf("food is empty. consume nothing... (TODO wait)\n");

		}
		pthread_mutex_unlock(&food_buf.mutex);
		//eating
		sleep(1);

	}
}

int main()
{

	//food_buf.mutex = PTHREAD_MUTEX_INITIALIZER; //这个只能用在定义的初始化阶段。https://blog.csdn.net/wocjj/article/details/7691391
	pthread_mutex_init(&food_buf.mutex, NULL);

	pthread_t consumer_pt[CONSUMERNUM]; //消费者线程
	pthread_t producer_pt[CONSUMERNUM]; //生产者线程
	//一半生产者生产汉堡
	for(int i = 0; i < PRODUCERNUM / 2 ; i++)
	{
		FOOD hamberger;
		hamberger.food_calorie = 1300;
		strcpy(hamberger.food_name , "HAMBERGER");
		pthread_create(&producer_pt[i] , NULL , &produce_food, (void *)&hamberger);
	}
	//一半生产者生产可乐
	for(int i = PRODUCERNUM / 2; i < PRODUCERNUM; i++)
	{
		FOOD coke;
		coke.food_calorie = 800;
		strcpy(coke.food_name , "Coke");
		pthread_create(&producer_pt[i] , NULL , produce_food, (void *)&coke);
	}

	//消费者进行消费
	for(int i = 0; i < CONSUMERNUM ; i++)
	{
		pthread_create(&consumer_pt[i] , NULL ,consume_food , NULL);
	}

	for(int i = 0; i < PRODUCERNUM ; i++)
	{
		pthread_join(producer_pt[i], NULL);
	}

	for(int i =0; i < CONSUMERNUM; i++)
	{
		pthread_join(consumer_pt[i], NULL);
	}

	return 0;
}


