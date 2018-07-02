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
			if(end - start > 0) //ls;;; ==>只处理一个，避免做无用malloc
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

	//处理最后. 
	if(end - start > 0) //避免ls;date;clear;  最后是;时做无用的malloc
	{
		arr[count] = (char*)malloc(LINELEN);
		strncpy(arr[count], start , end - start);
		arr[count][end - start] = '\0';
		count++;
	}


	*num = count;

}

int conver2List(const char *buff, char *list[], int *arg_num)
{
	printf("command:%slxrend\n",buff);//lxrend为了看清楚字符串最后是否有空格
	int num = 0;
	const char *end = buff;
	skipHeadBlank(&end);
	const char *start = end;

	while(*end != '\0') 
	{
		if(*end == ' ' || *end == '\t') 
		{
			if(end - start > 0)
			{
				list[num] = (char*)malloc(sizeof(char)*ARGLEN);
				strncpy(list[num] , start , end - start);
				list[num][end - start] = '\0';
				//printf("list[%d]: %s, left:%s\n",num, list[num],end);
				num++;
			}
			end++;
			skipHeadBlank(&end);
			start = end;
		}
		else
		{
			end++;
		}
	}
 
	if(end - start > 0)  ////避免ls;date;clear\t\t 最后是空格时， 做无用的malloc
	{
		list[num] = (char*)malloc(sizeof(char)*ARGLEN);
		strncpy(list[num] , start , end - start);
		list[num][end - start] = '\0';
		num++;

	}
	*arg_num = num;
	
	return 0;
}

int execute(char *args[])
{
	if(strcmp(args[0],"exit") == 0)//不会执行到，在main中就处理了exit
	{
		exit(123);
		printf("not run after exit\n");
	}
	if(execvp(args[0] , args) == -1) //对exit命令无效
	{
		printf("execvp error\n");
	}

	printf("not run here\n");
	exit(1);
}

int main()
{
	char line[LINELEN];
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

		//增加对同一行分号的支持。 分割成一个命令数组
		split2ArrByChar(line, ';',cmdArr,  &num_cmd);

		printf("command num:%d\n", num_cmd);
		//对命令数组进行处理，执行每一个名命令
		for(int i = 0; i < num_cmd && exitFlag != 1; i++)
		{
			const char *pc = cmdArr[i];
			skipHeadBlank(&pc);
			if(*pc == '\0')
			{
				printf("not command ,continue\n");
				continue;
			}

			conver2List(pc, list, &num_arg);
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

					//清理list
					for(int i = 0; i < num_arg ; i++)
					{
						if(list[num_arg] != NULL)
							free(list[num_arg]);
					}
					//printf("child return exit_code %d, signo %d\n", status >> 8, status & 0x008f );
					if(status >> 8 == 123)
						break;
				}
			}
		}
		//清理malloc的内存
		for(int i = 0; i < num_cmd ; i++)
		{
			if(cmdArr[i] != NULL)
				free(cmdArr[i]);
		}
	}
	printf("end main\n");
}