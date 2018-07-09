/*
哲学家问题。

哲学家就餐问题可以这样表述
假设有五位哲学家围坐在一张圆形餐桌旁，做以下两件事
情之一：吃饭，或者思考。吃东西的时候，他们就停止思考，思考的时候也停止吃东西。
餐桌中 间有一大碗意大利面，每两个哲学家之间有一只餐叉。
因为用一只餐叉很难吃到意大利面，所以 假设哲学家必须用两只餐叉吃东西。他们只能使用自己左右手边的那两只餐叉。
哲学家就餐问题 有时也用米饭和筷子而不是意大利面和餐叉来描述，因为很明显，吃米饭必须用两根筷子。

*/


/*
这里可以直接用互斥量来实现。 也可以使用信号量实现(二元信号量，也是用于互斥)。
*/


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define PHILOSOPHER_NUM	5

typedef struct chopstic
{
	int chopstic_no; //筷子编号
	pthread_mutex_t mutex;
}Chopstic;

Chopstic chopstics[PHILOSOPHER_NUM];



void * eating(void *arg)
{
	int no = *(int *)arg;//哲学家编号

	while(1)
	{
		/*
		这样没有限制处理，可能会造成死锁，互相等待的情况。
		解决方法：1.同一时间只能🈶一个人拿起左右筷子。
				 2.最多只能由四个人同时拿起筷子
				 3.单号哲学家先拿单号筷子，再拿双号。 双号哲学家先拿双号，再拿单号.
		*/
		printf("第%d个哲学家准备拿起筷子...\n", no);
		//先拿起右边筷子
		pthread_mutex_lock(&chopstics[no].mutex);
		//再拿起左边筷子
		pthread_mutex_lock(&chopstics[(no+1)%PHILOSOPHER_NUM].mutex);
		
		printf("第%d个哲学家拿到了筷子%d和筷子%d。开始吃饭。\n", no, chopstics[no].chopstic_no , chopstics[(no+1)%PHILOSOPHER_NUM].chopstic_no);

		sleep(1);

		//放下筷子
		pthread_mutex_unlock(&chopstics[no].mutex);
		pthread_mutex_unlock(&chopstics[(no+1)%PHILOSOPHER_NUM].mutex);


		printf("第%d个哲学家放下了筷子，开始思考...\n",no);

		sleep(1);

	}
}
int  main()
{

	pthread_t tid_philosopher[PHILOSOPHER_NUM];
	int philosopher_no[PHILOSOPHER_NUM]; //哲学家编号

	for(int i = 0; i < PHILOSOPHER_NUM; i++)
	{
		chopstics[i].chopstic_no = i;
		pthread_mutex_init(&chopstics[i].mutex, NULL);
	}

	for(int i = 0; i < PHILOSOPHER_NUM; i++)
	{
		philosopher_no[i] = i;
		pthread_create(&tid_philosopher[i], NULL, eating,&philosopher_no[i]);
	}

	for(int i =0; i < PHILOSOPHER_NUM; i++)
	{
		pthread_join(tid_philosopher[i], NULL);
	}



	return 0;
}