#include <stdio.h>
#include <math.h>
#include <string.h>
#define BUFLEN  4096


int str2int(char * str,int *number)
{
    char *ptr;
    ptr = str;
    int sum = 0;
    int numCount = 0;
    int op = 1;
    
    //judge 1st char.
    if (*ptr == '-')
    {
        op = -1;
        ptr++;
    }
    else if(*ptr == '+')
    {
        op = 1;
        ptr++;
    }
    else
    {
        op = 1;
    }
    
    while (*ptr != '\0')
    {
        if (*ptr < '0' || *ptr > '9')
        {
            return -1;
        }
        sum = sum * 10 + (*ptr -'0');
        numCount++;
        ptr++;
    }
    
    *number = sum;
    return 0;
}


//mode: 0 by number, 1 by byte
void tail_number(char * filename, int number, int mode)
{
    FILE *fp;
    char buffer[BUFLEN];
    memset(buffer, 0, BUFLEN);
    int num_read;
    if (strcmp(filename,"") == 0)
    {
        fp = stdin;
    }
    else
    {
        fp = fopen(filename, "r");
    }
    if (mode == 1)
    {
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        int min = number < file_size? number:file_size;
        fseek(fp, 0 - min, SEEK_END);
        num_read =fread(buffer, min, sizeof(char), fp);
//        for (int i = 0; i <= min; i++) {
//            if (i == min)
//            {
//                printf("min+%c#",buffer[i]);
//            }
//            else
//            {
//                printf("%c#",buffer[i]);
//            }
//        }
//        printf("\n");
        buffer[num_read] = '\0'; //注意加结束符。 fread 不会在最后加\0
        fputs(buffer, stdout);
        
    }
    else
    {
        int off_set = locate_file(fp,number);
        fseek(fp, off_set, SEEK_SET);
        while(fgets(buffer,BUFLEN,fp))
        {
            fputs(buffer, stdout);
        }
    }
    
    if (strcmp(filename,"") !=0)
        fclose(fp);
    
}

//定位从末尾位置往前的n行位置。
int locate_file(FILE * fp , int nline)
{
    char buf[BUFLEN];
    long file_size = 0;
    int num_read = 0;
    int off_end = 0;
    int line_num = 0;
    int min;
    int i = 0;
    
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    
    while (line_num < nline && off_end < file_size)
    {
        min = BUFLEN < file_size - off_end? BUFLEN : file_size - off_end;
        fseek(fp, 0 - off_end - min, SEEK_END);
        num_read = fread(buf,sizeof(char),min,fp);
        for (i = num_read -1; i >= 0; i--)
        {
            if (buf[i] == '\n')
            {
                line_num++;
                if (line_num == nline)
                {
                    break;
                }
            }
        }
        off_end = off_end + num_read-1 - i;
    }
    
    fseek(fp, 0, SEEK_SET); //恢复
    
    //TODO fread 会在最后buf加\0? 不会
    //直接往文件中写入结构体会如何？可以直接查看？还是只能结构化读取？
    
    return (file_size - off_end);
}

int main(int argc, char * argv[])
{
    int number; //line or byte number
    
    //head -n 10 file.  max argument 4
    if (argc  > 4)
    {
        printf("bad usage\n");
        return -1;
    }
    
    //head
    if (argc == 1)
    {
        //tail_number("",10,0);
        printf("bad usage\n"); //不接受从stdin读
        return -1;
    }
    
    
    if (strcmp(argv[1],"-n") == 0 ) //head -n num file
    {
        if (argc == 2)
        {
            printf("bad argument\n");
            return -1;
        }
        
        if (str2int(argv[2], &number) == -1)
        {
            printf("bad argument\n");
            return -1;
        }
        
        //head -n 10 file
        if (argc == 4)
        {
            tail_number(argv[3],number,0);
        }
        else //head -n 10
        {
            printf("bad argument\n");
            return -1;
            //tail_number("",number,0);
        }
    }
    else if (strcmp(argv[1],"-c") == 0) //head -c num file
    {
        if (argc == 2)
        {
            printf("bad argument\n");
            return -1;
        }
        if (str2int(argv[2], &number) == -1)
        {
            printf("bad argument\n");
            return -1;
        }
        
        //head -c 10 file
        if (argc == 4)
        {
            tail_number(argv[3],number,1);
        }
        else //head -c 10
        {
            printf("bad argument\n");
            return -1;
            //tail_number("",number,1);
        }
    }
    else if(argv[1][0] == '-') //head -10 file
    {
        char *tmp = argv[1] + 1;
        if (str2int(tmp, &number) == -1)
        {
            printf("bad usage\n");
            return -1;
        }
        if (argc == 2)//head -num
        {
            printf("bad argument\n");
            return -1;
            //tail_number("", number, 0);
        }
        else if(argc == 3) //head -num file
        {
            tail_number(argv[2], number, 0);
        }
        else
        {
            printf("bad usage \n");
            return -1;
        }
    }
    else
    {
        printf("bad usage\n");
        return -1;
    }
    
    return 0;
    
}
