#include <stdio.h>
#include <signal.h>

void  wake_up(int sig_no)
{
	printf("alarm received\n");
}
int main()
{
	printf("about to sleep for 3 seconds\n");

	signal(SIGALRM , wake_up);
	alarm(3);
	pause();
	printf("Moring coming soon?\n");
}