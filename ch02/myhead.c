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
void head_number(char * filename, int number, int mode)
{
    FILE *fp;
    char buffer[BUFLEN];
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
        fread(buffer, number, sizeof(char), fp);
        buffer[number] = '\0';
        fputs(buffer, stdout);
    }
    else
    {
        for (int i = 0; i < number; i++)
        {
            if(fgets(buffer,BUFLEN,fp) == NULL)
            {
                break;
            }
            fputs(buffer, stdout);
        }
    }
    
    if (strcmp(filename,"") !=0)
        fclose(fp);
    
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
        head_number("",10,0);
        return 0;
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
            head_number(argv[3],number,0);
        }
        else //head -n 10
        {
            head_number("",number,0);
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
            head_number(argv[3],number,1);
        }
        else //head -c 10
        {
            head_number("",number,1);
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
            head_number("", number, 0);
        }
        else if(argc == 3) //head -num file
        {
            head_number(argv[2], number, 0);
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
    
}
