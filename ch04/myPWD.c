#include <stdio.h>
#include <string.h>
#include <dirent.h>


void printPWD(char * dirNamePath)
{
	DIR * dir = NULL;
	struct dirent * dentry;

	char dirName[128];
	char parentPath[256];

	printf("dirNamePath:%s\n", dirNamePath);

	if((dir = opendir(dirNamePath)) == NULL)
	{
		printf("opendir error\n");
		return ;
	}

	long current_ino , parent_ino;

	dentry = readdir(dir);  //目录内容中的第一个条目是 .  第二个条目是..
	current_ino = dentry->d_ino;
	closedir(dir);
	dir = NULL;

	sprintf(parentPath , "../%s",dirNamePath);
	if((dir = opendir(parentPath)) == NULL)
	{
		printf("opendir error\n");
		return ;
	}

	while(dentry = readdir(dir))
	{
		if (strcmp(dentry->d_name,".") ==0)
		{
			parent_ino = dentry->d_ino;
		}
		if (dentry->d_ino == current_ino)
		{
			strcpy(dirName,dentry->d_name);
			break;
		}
	}

	closedir(dir);

	printf("%ld %ld %s %s\n",current_ino ,parent_ino ,dirName ,dentry->d_name);

	if(parent_ino != current_ino) //根目录停止递归 当前目录节点 == 父目录节点 
	{
		printf("%s\n", parentPath);
		printPWD(parentPath);
	}
	if(strcmp(dirName,".") != 0) //根目录 没有 dirName,消除最后的 /./Users/....
		printf("/%s", dirName);
}

int main(int argc, char*argv[])
{
	if (argc != 1)
		return -1;

	printPWD(".");
	printf("\n");

	return 0;
}
