#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int main()
{

	pid_t pid;

	if((pid = fork()) < 0)
	{
		printf("fork error");
		return -1;
	}
	else if(pid == 0)
	{
		printf("first child pid=%d\n", getpid());
		sleep(1);
		printf("first child, parent pid = %ld\n", (long)getppid());
		exit(0);
	}


	int pid2 = fork();

	if(pid2 < 0)
	{
		printf("fork error");
		return -1;
	}
	else if(pid2 == 0)
	{
		printf("second child pid=%d\n", getpid());
		sleep(3);
		printf("second child, parent pid = %ld\n", (long)getppid());
		exit(0);
	}


//waitpid 也是只能等待其直接子进程。但是可以指定某个子进程。
	printf("waitpid:%d\n", waitpid(pid2 , NULL, 0));
//wait只会等待其直接子进程. 如果有多个直接子进程，那么其第一个子进程退出，这个wait就返回。
//	printf("wait:%d\n",wait(NULL));
	/*
	if(waitpid(pid , NULL, 0) != pid)
	{
		printf("waitpid err\n");
		return -1;
	*/

	printf("over\n");

		return 0;

}
