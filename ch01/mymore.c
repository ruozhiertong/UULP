#include<stdio.h>
#define PAGELINE	24
#define LINELEN		512
void domore(FILE* fp);
int seemore();
int seemore2();
int main(int argc,char*argv[])
{
	FILE * fp;
    
    //test for getchar
//    char c ;
//    c= getchar();
//    {
//        printf("%d\n",c);
//        switcxh (c) {
//            case ' ':
//                printf("space\n");
//                break;
//            case '\n':
//                printf("enter \n");
//                break;
//            default:
//                printf("%d\n",c);
//                break;
//        }
//    }
    
	if(argc < 2)
    {
        printf("few arguments\n");
        return -1;

    }
    
	while(argc != 1)
	{
		if((fp = fopen(argv[--argc],"r")) == NULL)
		{
			return -1;
		}
		domore(fp);
		fclose(fp);
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
		puts(linedata);// print to stdout
		if(linecount == PAGELINE)
		{
			puts("\033[7m more? \033[m");//print the tip
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
	int commond;
	char c, aband;
    rewind(stdin);
	c = getchar(); //当stdin输入键入enter后，保存到缓冲区（\n也会保存），然后从缓冲区读取到c。getchar 知道第一个字符。 这里有一个问题存在，当按space 后，再按 enter是结束这次输入，但是我们getchar是读第一个字符后停止，而stdin缓冲区依然还有enter键，下次再次调用seemore的时候，会继续从缓冲区读，因为缓冲区还有值，也就是说我们还重新键入值，getchar 是从原先读后的值开始读，也就是enter键的值。所以提前return。 解决方法，1.刷新/清空stdin缓冲区，让其getchar 一个后，刷新,即清空缓冲区。使得下次进来的时候，stdin缓冲区是新的。2.getchar使用掉最后的\n, 不过这个只能是处理键入1个键时的情况， 即 x + enter。3.getch 直接从键盘输入,但是这个不是c标准库。
    switch (c)
	{
		case 'q':
			commond = 0;
            break;
		case '\n':
			commond = 2;
            break;
        case ' ':
			commond = 1;
            break;
		default:
			commond = 1;
            break;
	}
    //getchar();
    //fflush(stdin); 在vc下可以，在linux下不行。
    //setbuf(stdin, NULL); //不起作用
    //scanf("%*[^\n]%*c"); 会还有一个问题，如果这时只是一个enter键。这个在上面那个getchar吸收了，而这时scanf还要再等待输入，即如果要有enter 必须是enter enter。 后面的while那个一样的问题
   // while((aband = getchar()) != '\n' && c != EOF);
	return commond;
}
