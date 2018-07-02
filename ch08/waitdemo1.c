#include <stdio.h>
#include <unistd.h>

#define DELAY	10


void child_code(int sleepSecond)
{
	printf("in child process, pid=%d , sleep %d\n", getpid(), sleepSecond);

	sleep(sleepSecond);

	printf("child done . about to exit\n");

	exit(17);
}

void parent_code(int childPID)
{
	printf("in parent process, pid=%d, the child_pid=%d\n", getpid(), childPID);

	printf("parent will wait for child\n");

	int status_code = 0;
	int wait_ret = wait(&status_code);

	printf("done waiting for child_pid=%d, the wait returned=%d, the status_code=%d\n",childPID, wait_ret, status_code);

	printf("sizeof int :5d\n",sizeof(int));

	int child_exit_code = status_code >> 8;

	int child_sig_no = status_code & 0x007f;

	int child_core_dump = status_code & 0x0080;

	printf("child_exit_code=%d  child_sig_no=%d child_core_dump=%d\n",child_exit_code , child_sig_no, child_core_dump );
}

int  main()
{
	int newpid;

	newpid = fork();

	if(newpid == -1)
	{
		printf("fork error\n");
		return -1;
	}
	else if(newpid == 0)
	{
		child_code(DELAY);
	}
	else if(newpid > 0)
		parent_code(newpid);
	return 0;
}