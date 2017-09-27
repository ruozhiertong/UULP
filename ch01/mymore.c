#include<stdio.h>
#define PAGELINE	24
#define LINELEN		512
void domore(FILE* fp);
int seemore();
int seemore2();
int main(int argc,char*argv[])
{
	FILE * fp;
    
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
			commond = seemore();
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
