#include <stdio.h>
#include <stdlib.h> // for getenv

extern char** environ;

int main()
{

	int i =0;

	for(i = 0; environ[i] ;i++)
	{
		printf("%s\n", environ[i]);
	}
	printf("second\n");

	char* cp = getenv("LANG");

	if(cp != NULL)
		printf("%s\n",cp);
	return 0;
}

 