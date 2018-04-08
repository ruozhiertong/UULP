#include<sys/stat.h>
#include <stdio.h>
#include <dirent.h>

#define PARANUM 10

#define LSA 0x0001
#define LSI 0x0010
#define LSL 0x0100

// ls [option] [pathname] not ls [pathname] [option]
//ls -a -l -i
//ls -lai
int myls(char *path, char*mode[],int num)
{
    DIR  *dir;
    struct dirent *dentry;
    
    struct stat statinfo;

    
    int mod = 0x0000;
    char *p;
    
    char fullFileName[100];
    
    char * fileName;
    
    
    if((dir = opendir(path)) == NULL)
    {
        printf("could not open %s\n",path);
        return -1;
    }
    
    //check mode
    for (int i = 0; i < num; i++)
    {
        p = mode[i];
        
        p++; //skip '-' ;
        while (*p)
        {
            if(*p == 'a')
            {
                mod |= LSA;
            }
            else if (*p =='l')
            {
                mod |= LSL;
            }
            else if (*p =='i')
            {
                mod |= LSI;
            }
            else
            {
                printf("wrong param\n");
                return -1;
            }
            p++;
        }
    }
    
    while (dentry = readdir(dir))
    {
        
        if (mod & LSA)
        {
            fileName = dentry->d_name;
        }
        else
        {
            //隐藏文件不显示
            if (dentry->d_name[0] == '.')
            {
                continue;
            }
            else
            {
                fileName = dentry->d_name;
            }
        }
        strcpy(fullFileName,path);
        
        strcat(fullFileName,"/");
        
        strcat(fullFileName,fileName);
    
        if(stat(fullFileName,&statinfo) == -1)
        {
            printf("errot stat\n");
            return -1;
        }
        
        if (mod & LSI)
        {
            printf("inode_no:%lld\t",statinfo.st_ino);
        }
        
        if (mod & LSL)
        {
            printf("mode:%lld\tlink:%lld\tuid:%d\tgid:%d\tsize:%lld\ttime:%ld\tblock:%lld\tblocksize:%lld\t",statinfo.st_mode,statinfo.st_nlink,statinfo.st_uid , statinfo.st_gid,statinfo.st_size,statinfo.st_mtimespec.tv_sec,statinfo.st_blocks,statinfo.st_blksize);
        }
        
        printf("%s\n",fileName);
        
    }

    closedir(dir);
    
    
    
    
    return 0;
}

int main(int argc, char * argv[])
{
    char * mode[PARANUM];
    int modeCount = 0;
    char * pathName;
    
    if (argc == 1) // usage: ls
    {
        pathName = ".";
    }
    else
    {
        for (int  i = 1; i < argc; i++)
        {
            if (i == 1)
            {
                if (argv[i][0] == '-')
                {
                    pathName = ".";
                    mode[modeCount++] = argv[i];
                }
                else
                {
                    pathName = argv[i];
                }
            }
            else
            {
                if (argv[i][0] != '-')
                {
                    printf("bad usage! Usage: ls path -a -l\n");
                    return -1;
                }
                mode[modeCount++] = argv[i];
            }
        }
        
    }
    
    if (myls(pathName, mode, modeCount) == 0)
        return 0;
    else
        return -1;
}