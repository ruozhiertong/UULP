#include<stdio.h>
#include "curses.h"
#include <unistd.h> 
#include <termios.h> 
#include <assert.h>
#include <fcntl.h>
#define PAGELINE	24
#define LINELEN		512
void domore(FILE* fp);
int seemore();
int seemore2();
int main(int argc,char*argv[])
{
	FILE * fp;
    
    //    while (1) {
    //        seemore3();
    //    }
    
    //添加管道,重定向的处理。 如 ls -la | ./a.out。
    //处理一个argc = 1时，即将标准输入作为输入源，而非某个文件。
    
    
    if(argc == 1)
    {
        domore(stdin);
    }
    else
    {
        while(argc != 1)// 遍历每个参数/即文件名
        {
            if((fp = fopen(argv[--argc],"r")) == NULL)
            {
                return -1;
            }
            domore(fp);
            fclose(fp);
        }
        
    }
    
	return 0;
}

void domore(FILE* fp)
{
	char linedata[LINELEN];
	int linecount = 1;
	int commond = 1; //q=0, oneline=1 onepage=2
	while(fgets(linedata,LINELEN,fp)) //read line from fp
	{
		//puts(linedata);// print to stdout. puts print would add \n, use fputs
        fputs(linedata, stdout);
		if(linecount == PAGELINE)
		{
			commond = instant_get();
			switch (commond)
			{
				case 0:
					return ;
				case 1:
					linecount = PAGELINE - 1; //read one line
					break;
				case 2:
					linecount = 0; //read one page
					break;
			}
		}
		linecount++;
	}
}

//this is from uulp's ch01 , but it does not work.
int seemore2()
{
    int c;
    while((c=getchar()) != EOF)			/* get response	*/
    {
        printf("%d\n",c);
        if ( c == 'q')			/* q -> N		*/
            printf("=========q============\n");
        return 0; //这里直接返回是因为getchar会一直读stdin的缓冲区，直到\n，如果没有返回，还会继续读，会将我们按的enter键读入进来，因此这里我们只需要读第一个输入的键，而不需要enter结束键入的那个键。但是依然会有缓冲区残留\n的情况。
        if ( c == ' ' )			/* ' ' => next page	*/
            printf("=========1============\n");
            return 1;		/* how many to show	*/
        if ( c == '\n' )		/* Enter key => 1 line	*/
            printf("=========2============\n");
            return 2;
    }
    printf("%d\n",c);
    return 0;
}

//key point: how to read /n space from stdin.
int seemore()
{
	char c;
    //puts("\033[7m more? \033[m");//print the tip
    fputs("\033[7m more? \033[m", stdout);
    //just q \n space in use
    //would be a problem when redirect 重定向或管道.
    //it could not accept by keybord as stdin. because of redirect, stdin now is the stdout of the last command
    //so we can ues /dev/tty as input.
    //rewind(stdin); //avoid the result of the last input.
    //c = getchar();
    FILE *fpInput;
    fpInput = fopen("/dev/tty", "r");
    rewind(fpInput); //虽然fegtc不处理\n，但是如果输入多个字符，依然会读取上一次enter之后的值。所以还是rewind
    c = fgetc(fpInput); //fgetc 不处理\n.
    while(c!=EOF)
    {
        switch (c)
        {
            case 'q':
                return 0;
            case '\n':
                return 2;
            case ' ':
                return 1;
        }
        rewind(fpInput);
        c = fgetc(fpInput); //use first input char
    }
    return 0;
}


int seemore3()
{
    //处理在显示more时，按键之后还需要enter之后才能处理。想直接按一个键之后处理，使用read。read也不行，read也要等enter之后才处理。read也会受到上次输入的影响。read其实也是保存在缓冲区，但是这个是内核管理的缓冲区，和getchar这样的缓冲区不太一样，getchar是由C标准库处理。因为这个缓冲区，因此read方案失败。
    //网上推荐getch。不过getch不是标准库。要使用curses库。
    //还是失败了。
    //方法3：修改终端设置。
    char c;
    initscr();
    c = getch();
    endwin();
    
    printf("%d\n",c);
    
    while (1) {
        printf("%d\n",c);
        switch (c) {
                case 'q':
                    return 0;
                case '\n':
                    return 2;
                case ' ':
                    return 1;
        }
        initscr();
        c = getch();
        endwin();
    }
    
    
    
    return 0;
}

int instant_get()
{
    int c = 0;
    char buf[10];
    int ret = 0;
    int res = -1;
    struct termios org_opts, new_opts;
    
    fputs("\033[7m more? \033[m", stdout);

    int fd;
    //FILE *pf;
    //pf = fopen("/dev/tty", "r"); 使用pf来fgetc也可以。不过不想开那么多的文件指针，既然有了fd，直接用这个进行read就行。
    fd = open("/dev/tty",O_RDONLY);
    res = tcgetattr(fd, &org_opts);
    //res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);
    memcpy(&new_opts, &org_opts, sizeof(org_opts));
    //不回显 & 立即返回
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE);
    //tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    tcsetattr(fd, TCSANOW, &new_opts);
    
    while((c=read(fd, buf, 10)))
    {
        //printf("%d\n",c); //因为设置了终端，所以终端每次只能读一个字符，read的时候实际读的也只是一个字符的。
        c = buf[0];
        if (c == 'q')
        {
            ret = 0;
            break;
        }
        else if(c == ' ')
        {
            ret = 1;
            break;
        }
        else if(c == '\n')
        {
            ret = 2;
            break;
        }
    }
    
    tcsetattr(fd, TCSANOW, &new_opts);
    //res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    
    close(fd);
    //fclose(pf);
    
    return ret;
}
