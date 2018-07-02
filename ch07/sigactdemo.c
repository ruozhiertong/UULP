#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define INPUTLINE	81


void inthandler(int sig_no)
{
	printf("Called with signal %d\n",sig_no);
	sleep(sig_no);
	printf("done handling signal  %d\n",sig_no);

}
int main()
{

	char input[INPUTLINE];

	struct sigaction newHandler;

	sigset_t blockedSet;


	newHandler.sa_handler = inthandler;

	newHandler.sa_flags = SA_RESTART| SA_RESETHAND;

	sigemptyset(&blockedSet);
	sigaddset(&blockedSet , SIGQUIT);

	newHandler.sa_mask = blockedSet;

	if(sigaction(SIGINT, &newHandler , NULL) == -1)
	{
		printf("sigaction error\n");
		return -1;
	} 
	else
	{
		while(1)
		{
			fgets(input, INPUTLINE , stdin);
			printf("input: %s\n", input);
		}
	}



	return 0;
}