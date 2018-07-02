#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
	int fd;
	int pid;

	char msg1[] = "Test 1 2 3 ...\n";
	char msg2[] = "Hello ,hello\n";

	if((fd = creat("testfile", 0644)) == -1)
	{
		printf("create fail\n");
		return -1;
	}

	if(write(fd, msg1,strlen(msg1)) == -1)
	{
		printf("write msg1 fail\n");
		return -1;
	}

	if((pid =fork()) == -1)
	{
		printf("fork error\n");
		return -1;
	}
	else if(pid == 0)
	{
		sleep(5);

		if(write(fd, msg1,strlen(msg1)) == -1)
		{
			printf("write  msg2 fail\n");
			return -1;
		}

		if(close(fd) == -1)
			printf("in child close fail\n");
	}
	else
	{
		if(write(fd, msg2,strlen(msg2)) == -1)
		{
			printf("write  msg2 fail\n");
			return -1;
		}

		if(close(fd) == -1)
			printf("in parent close fail\n");

		wait(NULL);
	}
	printf("end main\n");
	return 0;
}