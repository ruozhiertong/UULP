#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h> // for fork

int main()
{

	int status;


	int pid;

	pid = fork();

	if(pid < 0)
	{
		printf("fork error\n");
		return -1;		
	}

	if(pid == 0)
	{
		printf("in child. Its pid: %d\n", getpid());
		int child_child_pid = fork();

		if(child_child_pid == 0)
		{
			printf("in child's child. Its pid: %d\n", getpid());
		}
		else
		{
			sleep(2);
			int ch_ch_pid = wait(NULL);
			printf("in child wait %d\n", ch_ch_pid);

		}
	}
	else
	{
		printf("in parent. Its pid: %d\n",getpid());
		//只会等待其直接子进程.
		int child_pid = wait(NULL);
		printf("%d\n", child_pid);
	}

/*
	//waitpid无法等待非子进程的进程。这里等待一个非子进程的进程，会返回错误。
	if(waitpid(9789, &status, 0) == -1)
	{
		printf("error\n");
	}
	else
	{
		printf("success\n");
	}
	*/
	return 0;
}