#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("error. Ussage : write0 ttyname\n");
		return -1;
	}

	int fd = open (argv[1], O_WRONLY);
	if(fd == -1)
	{
		printf("open  error\n");

		return -1;
	}
	char buf[512];
	while(fgets(buf, 512, stdin) != NULL)
	{
		if(write(fd, buf, strlen(buf)) == -1)
			break;
	}

	close(fd);
	return 0;
}