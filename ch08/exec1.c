#include <stdio.h>
#include <unistd.h>
int main()
{
	char* arglist[3];

	arglist[0] ="Hello"; //用于ps 显示的名称

	arglist[1] = "-l";

	arglist[2] = 0;

	printf("*** About to exec ls -l\n");

	execvp("./test", arglist);

	printf("*** ls is done, bye!\n");

}