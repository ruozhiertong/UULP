#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h> // for strncmp

#define INPUTLINE	81

void inthandler(int sig_no)
{
	printf("Received signal %d .. waiting\n", sig_no);
	sleep(2);
	printf("Leaving inthandler\n");
}

void quithandler(int sig_no)
{
	printf("Received signal %d .. waiting\n", sig_no);
	sleep(3);
	printf("Leaving quithandler\n");
}

int main()
{
	char input[INPUTLINE];

	signal(SIGINT, inthandler);

	signal(SIGQUIT, quithandler);

	int nchars;
	do
	{
		printf("\nType message\n");
		nchars = read(0, input, INPUTLINE - 1);
		printf("Input nums %d\n",nchars );
		if(nchars == -1)
		{
			printf("read return an error\n");
			return -1;
		}
		else
		{
			input[nchars] = '\0';
			printf("Your input is : %s\n", input);
		}

	}while(strncmp(input, "quit" , 4) != 0);

	return 0;
}