#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

/*
和8.4.c区别：https://bbs.csdn.net/topics/260023754
*/
int main()
{
	FILE* fp;
	int pid;

	char msg1[] = "Test 1 2 3 ...\n";
	char msg2[] = "Hello ,hello\n";

	if((fp = fopen("testfile2" , "w")) == NULL)
	{
		printf("fopen fail\n");
		return -1;
	}

	fprintf(fp, "all %s", msg1);

	if((pid =fork()) == -1)
	{
		printf("fork error\n");
		return -1;
	}
	else if(pid == 0)
	{
		sleep(5);

		fprintf(fp, "child %s", msg1);

		fclose(fp);
	}
	else
	{
		fprintf(fp, "parent %s", msg2);

		fclose(fp);

		wait(NULL);
	}
	printf("end main\n");
	return 0;
}