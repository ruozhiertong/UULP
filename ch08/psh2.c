#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define LINELEN	81
#define MAXARGS	20
#define ARGLEN	20
#define MAXCMD	20


void skipHeadBlank(const char ** pc)
{
	const char *ptr = *pc;
	while(*ptr != '\0')
	{
		if(*ptr == ' ' || *ptr == '\t')
			ptr++;
		else
			break;
	}
	*pc = ptr;
}

//字符串src按指定的分隔符splitchar进行分割。存储为字符串数组 arr，以及分割成的个数 num  
void split2ArrByChar(const char *src, char splitchar, char ** arr, int *num)
{
	if(src == NULL)
		return;

	const char *end = src;
	const char *start;
	int count = 0;

	//忽略开头的空格。
	skipHeadBlank(&end);
	start = end;
	while(*end != '\0')
	{
		if(*end == splitchar)
		{
			//printf("size %ld\n", end - start);
			if(end - start > 0) //ls;;; ==>只处理一个
			{
				arr[count] = (char*)malloc(LINELEN);
				strncpy(arr[count], start , end - start);
				arr[count][end - start] = '\0';
				count++;
			}

			end++;
			skipHeadBlank(&end);
			start = end;
		}
		else
			end++;
	}

	//处理最后
	if(end - start > 0)
	{
		arr[count] = (char*)malloc(LINELEN);
		strncpy(arr[count], start , end - start);
		arr[count][end - start] = '\0';
		count++;
	}


	*num = count;

}

int conver2List(const char **buff, char *list[], int *arg_num)
{
	printf("%s\n",*buff);
	int num = 0;
	const char *end = *buff;
	skipHeadBlank(&end);
	const char *start = end;

	while(*end != '\0' && *end != ';')  //增加同一行中对；的支持。；表示一个命令的结束。 ls ; date 支持一行中运行多个命令。
	{
		if(*end == ' ' || *end == '\t') 
		{
			list[num] = (char*)malloc(sizeof(char)*ARGLEN);
			strncpy(list[num] , start , end - start);
			list[num][end - start] = '\0';
			//printf("list[%d]: %s, left:%s\n",num, list[num],end);
			num++;
			end++;
			skipHeadBlank(&end);
			start = end;
		}
		else
		{
			end++;
		}
	}
 
	//bug: ；在最后时，会有问题。  空格在最后时也会有问题。
	//已解决。
	//对; 或 \0结束时是否还有合法的字符进行检测。 有，则处理。
	//if(*end == '\0')
	//{
		//判断是否是合理的字符。
		int valid = 0; //有效字符
		const char *tmp = start;
		while(*tmp != '\0' && *tmp != ';')
		{
			if(*tmp !=' ' && *tmp != '\t')
			{
				valid = 1;
				break;
			}
			tmp++;
		}
		//还有合法的字符，说明还有命令
		if(valid)
		{
			tmp = start;
			while(*tmp != '\0' && *tmp != ';')  //增加同一行中对；的支持。
			{
				tmp++;
			}
			list[num] = (char*)malloc(sizeof(char)*ARGLEN);
			strncpy(list[num] , start , tmp - start);
			list[num][tmp - start] = '\0';
			num++;
		}

	//}
	
	*arg_num = num;

	if(*end == ';') //skip ;
		end++;
	
	*buff = end;
	return 0;
}

int execute(char *args[])
{
	if(strcmp(args[0],"exit") == 0)
	{
		exit(123);
		printf("not run after exit\n");
	}
	if(execvp(args[0] , args) == -1)
	{
		printf("execvp error\n");
	}

	printf("not run here\n");
	exit(1);
}

int main()
{
	char line[81];
	char *list[MAXARGS];
	int num_arg;
	int exitFlag = 0;
	char * cmdArr[MAXCMD];
	int num_cmd = 0;

	memset(cmdArr , 0, MAXCMD);
	while(exitFlag != 1)
	{
		printf("psh#");
		if(gets(line) == NULL)
		{
			printf("end of file before char input, break"); //continue\n");
			break;
			//continue;
		}
		/*
		先将一行的输入按照分号分割，每个分号是一个命令。shell支持一行多个命令，用分号分割。
		然后出炉每个命令，将其转成数组。
		exec 执行。
		*/


		split2ArrByChar(line, ';',cmdArr,  &num_cmd);

		printf("%d\n", num_cmd);
		for(int i = 0; i < num_cmd && exitFlag != 1; i++)
		{
			printf("%s\n", cmdArr[i]);

			const char *pc = cmdArr[i];

			skipHeadBlank(&pc);
			if(*pc == '\0')
			{
				printf("not command ,continue\n");
				continue;
			}

			while(*pc != '\0')
			{
				conver2List(&pc, list, &num_arg);
				printf("num_arg= %d\n", num_arg);
				if(num_arg > 0)
				{
					if(strcmp(list[0], "exit") == 0)
					{
						exitFlag = 1;
						break;		//直接在main进程进行拦截更好，不用fork。
					}
					list[num_arg] = NULL;
					int pid = fork();
					if(pid < 0)
					{
						printf("fork error\n");
						return -1;
					}
					else if(pid == 0)
					{
						execute(list);

					}else if (pid > 0)
					{
						signal(SIGINT,SIG_IGN);
						int status;
						wait(&status);
						//printf("child return exit_code %d, signo %d\n", status >> 8, status & 0x008f );
						if(status >> 8 == 123)
							break;
					}
				}
			}

		}
		for(int i = 0; i < num_cmd ; i++)
		{
			if(cmdArr[i] != NULL)
				free(cmdArr[i]);
		}
	}
	printf("end main\n");
}