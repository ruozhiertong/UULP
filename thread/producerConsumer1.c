/*
多线程生产者，消费者模型。
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
*/


/*
producerConsumer1.c
带条件变量。用于线程间同步。

多线程/多进程 存在竞争(非原子操作) ---导致---> 顺序不一致(数据不一致)-----需要----> 同步，互斥----会用到---->锁/互斥量 、条件变量、 信号量 -----这些手段可能产生----->死锁，饥饿的问题。
现在各OS上基本都支持：
信号量/信号灯(可用于同步 互斥。同步信号量，互斥信号量) . p/v semwait/semsignal  API：sem_wait/sem_post 操作.当用作互斥量时，其值为1。当用作同步量式，和条件变量类似，只是不用像条件变量那样专门设置一个条件，额外去判断条件。其信号量本身就是一个条件，内部就有对这个条件(信号量)的判断。
互斥量/互斥锁（用于互斥）. 一般说的锁，就是指互斥量/互斥锁。   还有读写锁(有同步，互斥的功能，可以用互斥量和条件变量实现)，自旋锁。
条件变量(用于同步，要配合互斥量)

*/

#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define FOODNUMMAX	4
#define CONSUMERNUM	2
#define PRODUCERNUM	2	

typedef struct food
{
	char food_name[81]; //food名称
	int  food_calorie;  //卡路里
}FOOD;

typedef struct food_buf
{
	pthread_mutex_t mutex;
	pthread_cond_t cond; //只用一个条件变量来处理缓存满和空的情况。缺点就是 可能线程会经常被虚假唤醒。也有可能应该被唤醒的没有被唤醒，而一直唤醒的是那个不想唤醒的线程，可能导致饥饿(当然这个唤醒调度由系统负责，具体哪个线程应该要被唤醒，我们无能为力)，甚至可能导致死锁。
						  // 可以用两个条件变量 cond_full cond_empty 分别控制。

	pthread_cond_t  cond_full;
	pthread_cond_t cond_empty;
	int food_num;
	FOOD food_arr[FOODNUMMAX];
}FOOD_BUF; //缓冲区

FOOD_BUF food_buf;
//food_buf.mutex = PTHREAD_MUTEX_INITIALIZER;



int testTime=0;

void * produce_food(void * arg)
{
	FOOD product = *(FOOD*)arg;
	while(1)
	{
		//后十次主要由消费者消费
		while(testTime>=10 && testTime < 20)
		{
			sleep(1);
			testTime++;

		}


		pthread_mutex_lock(&food_buf.mutex);

/*
		//这样修改,和不带条件变量没什么大进步，只进步一点点。只是避免一个线程中过多的进行空转(不满足条件时阻塞)，但是不能避免下次进来时，还是条件不满足，造成空转。应该用while检测。
		if(food_buf.food_num < FOODNUMMAX)
		{
			...
		}
		else
		{
			printf("%ld food is full. produce nothing. To wait\n", pthread_self());
			pthread_cond_wait(&food_buf.cond, &food_buf.mutex);
		}
*/
/*
		//用if 而不是while，可能会造成虚假唤醒。 虚假唤醒后， 可能会造成逻辑上的错误，或者其他问题。 如这里，可能会造成数组越界。
		//因为虚假唤醒后，其实条件还是没有满足，然后你唤醒后继续执行，就可能导致问题。
		if(food_buf.food_num >= FOODNUMMAX)
		{
			printf("%ld food is full. produce nothing. To wait\n", pthread_self());
			pthread_cond_wait(&food_buf.cond, &food_buf.mutex);
		}
		//produce
		printf("%ld producer is producing %d %s , its calorie is %d\n",pthread_self(),food_buf.food_num + 1 , product.food_name, product.food_calorie);
		......
		......
*/
		//使用条件变量
		//while避免虚假唤醒. 虚假唤醒：应该在特定情况下被唤醒，而有时候却被唤醒，导致虚假唤醒。因此用while进行判断。
		while(food_buf.food_num >= FOODNUMMAX)
		{
			printf("%ld food is full. produce nothing. To wait\n", pthread_self());
			pthread_cond_wait(&food_buf.cond, &food_buf.mutex);
		}

		//produce
		printf("%ld producer is producing %d %s , its calorie is %d\n",pthread_self(),food_buf.food_num + 1 , product.food_name, product.food_calorie);
		food_buf.food_arr[food_buf.food_num].food_calorie = product.food_calorie;
		strcpy(food_buf.food_arr[food_buf.food_num].food_name, product.food_name);
		food_buf.food_num++;
		pthread_cond_signal(&food_buf.cond);

		
		pthread_mutex_unlock(&food_buf.mutex);

		//produce
		sleep(1);


	}
}


void *consume_food(void *arg)
{
	while(1)
	{
		//前十次主要由生产者生产
		while(testTime < 10)
		{
			sleep(1);
			testTime++;
		}
		pthread_mutex_lock(&food_buf.mutex);


		while(food_buf.food_num <= 0)
		{
			printf("food is empty. consume nothing...  wait\n");
			pthread_cond_wait(&food_buf.cond, &food_buf.mutex);
		}


		//consume
		printf("%ld consumer is consuming %d %s , its calorie is %d\n",pthread_self(), food_buf.food_num, food_buf.food_arr[food_buf.food_num - 1].food_name, food_buf.food_arr[food_buf.food_num - 1].food_calorie);
		food_buf.food_num--;
		pthread_cond_signal(&food_buf.cond);

		pthread_mutex_unlock(&food_buf.mutex);
		//eating
		sleep(1);

	}
}


/*
关于条件变量 wait 和 signal死锁的情况：（死锁可能发生在互斥量mutex上，也有可能发生在条件变量上）
pthread_cond_signal  虚假唤醒后，信号丢失 导致所有线程都在wait，死锁
5个消费者， 3个生产者， 公共资源buf 最大长度为1。
A.一开始5个消费者共同执行，此时长度为buf0，此时5个消费线程都wait。
B.生产者生产，此时达到总长度1，并signal。
C.此时问题出现了，如果另一个生产者在消费者接收到signal信号前运行，那么此时生产者发现长度为1，满了，那么就进入wait状态了。
当signal过来时，如果被这个生产者接收，而生产者此时发现长度还是1，满了，又进入wait状态。 后续的生产者也进入了wait状态。
D.此时全部线程在wait，死锁了。


所以，虽然条件变量 wait signal 以及用while避免虚假唤醒，但是还是存在死锁的状况。


==》方法A:避免该被notify的没被notify，可以用全员notify。 不过这个缺点就是会使一些不该被notify的notify，然后重新判断，wait。虽然有些不好，但是解决了该notify却没被notify的问题。


==》方法B:以上情况都是编程错误导致的，避免这样互相wait，应该消费者的wait由生产者signal，生产者的wait由消费者signal。 两者
不应该用同一个条件变量。即用不同的条件变量。


*/


//改进版，避免死锁。
void * produce_food2(void * arg)
{
	FOOD product = *(FOOD*)arg;
	while(1)
	{
		//后十次主要由消费者消费
		while(testTime>=10 && testTime < 20)
		{
			sleep(1);
			testTime++;

		}
		pthread_mutex_lock(&food_buf.mutex);
		while(food_buf.food_num >= FOODNUMMAX)
		{
			printf("%ld food is full. produce nothing. To wait\n", pthread_self());
			pthread_cond_wait(&food_buf.cond_full, &food_buf.mutex);
		}

		//produce
		printf("%ld producer is producing %d %s , its calorie is %d\n",pthread_self(),food_buf.food_num + 1 , product.food_name, product.food_calorie);
		food_buf.food_arr[food_buf.food_num].food_calorie = product.food_calorie;
		strcpy(food_buf.food_arr[food_buf.food_num].food_name, product.food_name);
		food_buf.food_num++;
		pthread_cond_signal(&food_buf.cond_empty);

		
		pthread_mutex_unlock(&food_buf.mutex);

		//produce
		sleep(1);


	}
}

//改进版，避免死锁。
void *consume_food2(void *arg)
{
	while(1)
	{
		//前十次主要由生产者生产
		while(testTime < 10)
		{
			sleep(1);
			testTime++;
		}
		pthread_mutex_lock(&food_buf.mutex);


		while(food_buf.food_num <= 0)
		{
			printf("food is empty. consume nothing...  wait\n");
			pthread_cond_wait(&food_buf.cond_empty, &food_buf.mutex);
		}


		//consume
		printf("%ld consumer is consuming %d %s , its calorie is %d\n",pthread_self(), food_buf.food_num, food_buf.food_arr[food_buf.food_num - 1].food_name, food_buf.food_arr[food_buf.food_num - 1].food_calorie);
		food_buf.food_num--;
		pthread_cond_signal(&food_buf.cond_full);

		pthread_mutex_unlock(&food_buf.mutex);
		//eating
		sleep(1);

	}
}



int main()
{

	//food_buf.mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&food_buf.mutex, NULL);
	pthread_cond_init(&food_buf.cond, NULL);

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


