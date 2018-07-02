#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("befor:my pid is %d\n", getpid());

	fork();
	fork();
	fork();
	printf("after:my pid is %d\n", getpid());
}