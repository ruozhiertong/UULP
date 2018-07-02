/*
psh:promoting shell
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> //for exit()

#define MAXARGS	20
#define ARGLEN	81


int execute(char *args[])
{
	execvp(args[0] , args);

	printf("not run here\n");
	exit(1);
}

int main()
{

	char args[MAXARGS + 1][ARGLEN];

	char *argList[MAXARGS + 1];

	int num_args = 0;

	for(int i = 0; i < MAXARGS; i++)
	{
		printf("Arg[%d]?\n", num_args);
		gets(args[num_args]);
		if(args[i][0] == 0) // 提前结束
		{
			break;
		}
		argList[num_args] = args[num_args];
		num_args++;
	}

	if(num_args > 0)
	{
		argList[num_args] = NULL;
		execute(argList);
	}

	printf("not run here or num_args == 0\n");
	return 0;
}