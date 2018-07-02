#include <stdio.h>
#include <unistd.h>
#include <termios.h>


int main()
{
	struct termios ttyinfo;

	if(tcgetattr(STDIN_FILENO,&ttyinfo) == -1)
	{
		printf("tcgetattr error\n");
		return -1;
	}

    //ttyinfo.c_lflag &= ~ICANON; //设置成非规范模式，无缓冲。
    ttyinfo.c_cc[VMIN] = 0;
	ttyinfo.c_cc[VTIME] = 10; //超时设置要配合VMIN.  https://blog.csdn.net/jazzsoldier/article/details/55508227
	if(tcsetattr(STDIN_FILENO,TCSANOW, &ttyinfo) == -1)
	{
		printf("tcsetattr error\n");
		return -1;
	}

	printf("input something\n");

	char text[81];
	scanf("%s",text);
    
    char ch;
    
    ch = getchar();


	printf("end\n");

	return 0;
}
