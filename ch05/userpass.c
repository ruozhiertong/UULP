#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main()
{

	int fd;

	char user[81];
	char pass[81];
	printf("please input usernmae\n");
	scanf("%s",user);

	struct  termios attribs;

	//对标准输入进行设置。 STDIN_FILENO <==> 0   stdin ==> FILE*
	if(tcgetattr(STDIN_FILENO, &attribs) == -1)
	{
		printf("tcgetattr error \n");
		return -1;
	}

	attribs.c_lflag &= ~ECHO;

	if(tcsetattr(0 , TCSANOW, &attribs) == -1)
	{
		printf("tcsetattr error \n");
		return -1;
	}

	printf("please input pass\n");

	scanf("%s",pass);


	printf("username:%s  pass:%s\n", user, pass);


	//修改回去
	attribs.c_lflag |= ECHO;

	if(tcsetattr(0 , TCSANOW, &attribs) == -1)
	{
		printf("tcsetattr error \n");
		return -1;
	}


	printf("show something about ttyinfo\n");

	printf("The erase character is ascii %d , Ctrl-%c\n",attribs.c_cc[VERASE], attribs.c_cc[VERASE]-1 + 'A');

	printf("The kill character is ascii %d , Ctrl-%c\n",attribs.c_cc[VKILL], attribs.c_cc[VKILL]-1 + 'A');

	return 0;
}
