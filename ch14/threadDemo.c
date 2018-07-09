#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int count = 0;
void *func1(void *arg)
{

	printf("in thread1, it print hello evey 2 sec\n");


	while(1)
	{
		printf("hello %d\n",count++);
		sleep(2);
	}

}


void *func2(void *arg)
{

	printf("in thread2, it would exec ls \n");
	sleep(6);

	int pid = fork();

	if(pid < 0)
	{
		printf("fork err\n");
		exit(0);
	} else if(pid == 0)
	{
		printf("child's pid =%d, parent pid=%d\n", getpid(), getppid());
	}
	else
	{
		printf("in parent, the child pid=%d, pid=%d\n", pid, getpid());
	}
	//execlp("ls", "ls" , NULL);

	//printf("not run here\n");
}

int main()
{

	pthread_t t1 , t2;


	pthread_create(&t1, NULL, &func1 , NULL);

	pthread_create(&t2, NULL, &func2 , NULL);

	sleep(30);

	printf("after 30s, pid=%d\n", getpid());
	//因为线程2用exec 代替了当前的进程，因此，当前进程被替换了，也就消失了。 如果想让某个线程exec，而本身进程不被替换，那么
	//可以用fork。
	printf("it would not run here because of func2\n");

	return 0;
}