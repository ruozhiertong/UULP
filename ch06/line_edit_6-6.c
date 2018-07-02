#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>


//单行 处理
void single_line_edit()
{
	char line[81]; //每行最多81字符。

	char ch;
	int count = 0;
	while((ch=getchar()) != '\n')
	{
		//printf("%d %c\n", ch, ch);

		if(ch != 127)// 127 删除键的ASCII码
		{
			printf("%c", ch);
			line[count++] = ch;
		}
		else if(ch == 127)
		{
			printf("\b\040\b");
			//line[count--] = '\040'; // \040 八进制 空格 
			count--;
		}

	}

	printf("\ncount %d\n", count);
	line[count++] = '\0';
	printf("%s\n", line);
}

//多行 处理
void multi_line_edit()
{
	char buf[1024]; //每行最多81字符。

	char ch;
	int count = 0;
    
	//while((ch=getchar()) != EOF) //对于终端EOF由 Ctrl+D模拟
    while((ch=getchar()) != 4) //非规范模式下，Ctrl-D 居然是 ASCII 4， 在非规范模式下的终端Ctrl-D并非是EOF
    {                          //规范模式下，Ctrl-D 是 ASCII -1，即EOF。
		//printf("%d %c\n", ch, ch);
		if(ch != 127)// 127 删除键的ASCII码
		{
			printf("%c", ch);
			buf[count++] = ch;
		}
		else if(ch == 127)
		{
            if(buf[count -1] != '\n')
            {
                printf("\b\040\b");
                //buf[count--] = '\040'; // \040 八进制 空格
                count--;
            }

		}

	}

	printf("\ncount %d\n", count);
	buf[count++] = '\0';
	printf("%s\n", buf);
}



int main()
{

	struct termios ttyinfo;
	struct termios oldttyInfo;

	if(tcgetattr(0, &oldttyInfo) == -1)
	{
		printf("tcgetattr error\n");
		return -1;
	}

	memcpy(&ttyinfo, &oldttyInfo, sizeof(struct termios));

	ttyinfo.c_lflag &= ~ICANON; //取消缓冲模式/规范模式。 即处于 非规范模式。

	ttyinfo.c_lflag &= ~ECHO; //关闭回显

	//ttyinfo.c_cc[VEOF] = 'D' -'A' + 1;

	//c_cc[] 数组的索引表示具体啥功能，  数组的值 映射到键盘字符。 1 -> A 2 -> B 3 -> C 4 -> D 以此类推。
	printf("The eof character is ascii %d (base A) , Ctrl-%c\n",ttyinfo.c_cc[VEOF], ttyinfo.c_cc[VEOF] -1 + 'A');


	if(tcsetattr(0, TCSANOW, &ttyinfo) == -1)
	{
		printf("tcsetattr error\n");
		return -1;
	}

	printf("The eof character is ascii %d (base A), Ctrl-%c\n",ttyinfo.c_cc[VEOF], ttyinfo.c_cc[VEOF] -1 + 'A');


	single_line_edit();
	multi_line_edit();

	//恢复终端设置
	if(tcsetattr(0, TCSANOW, &oldttyInfo) == -1)
	{
		printf("tcsetattr error\n");
		return -1;
	}


	return 0;
}