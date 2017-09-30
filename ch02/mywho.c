#include <stdio.h>
#include <utmpx.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ttyent.h>
#include <pwd.h>
#define UTMP_FILE   "/var/run/utmpx"

void showInfo(struct utmpx whoinfo);
int main(int argc, char* argv[])
{
    //int fd;
    //mac os中 utmp 已经弃用，改用utmpx.而且读取方式不是直接read，通过api，endutxent。
    //struct utmp whoinfo;
    //fd = open(UTMP_FILE , O_RDONLY);
//    while (read(fd,&whoinfo,sizeof(struct utmpx)))
//    {
//        showInfo(whoinfo);
//    }
    //close(fd);
    struct utmpx *whoInfo;
    
    struct passwd *pwd;
    
    char ttybuf[100];
    char *ptty;
    FILE * fp;
    
    fp=popen("tty","r"); //利用管道将shell命令执行结果传回fp。
    fgets(ttybuf,sizeof(ttybuf),fp);
    printf("%s\n",ttybuf); // "/dev/ttys003\n"
    fclose(fp);
    ttybuf[strlen(ttybuf) - 1] = '\0'; // replace \n . "/dev/ttys003\0"
    ptty = ttybuf + 5; //abandon /dev
    printf("%s\n",ptty);
    
    pwd = getpwuid(getuid()); //用户登录时的ID
    printf("当前实际用户uid用户名为：%s\n", pwd->pw_name);
    
    pwd = getpwuid(geteuid()); //操作时的ID
    printf("(在设置chmod u+s后有效)当前有效用户euid的用户名为：%s\n", pwd->pw_name);
    
    printf("当前登录的用户 %s\n", getlogin());
    

    
    while ((whoInfo = getutxent()))
    {
        // who am i 只打印当前登录用户的当前终端。
        if (argc == 3 && strcmp(argv[1],"am") == 0 && strcmp(argv[2],"i") == 0)
        {
            //使用ptty过滤掉非当前的终端。有个小差异，如果在xcode的控制台下，显示的是xcode的控制台终端，
            //而who读取的文件中记录的只有terminal的终端，没有xcode下的终端，who是不显示xcode的终端。
            //会造成在xcode下运行who am i 显示为空。实际上应该显示xcode的终端和用户名等。
            if (whoInfo->ut_type != USER_PROCESS || strcmp(ptty, whoInfo->ut_line) != 0)
                continue;
            printf("%s\t%s\n",whoInfo->ut_user,whoInfo->ut_line);
            break;
            
        }
        showInfo(*whoInfo);
    }
    
//    
//    //停止 ps查看pid。 会发现这个pid和bash的pid相差1. 应该是bashfork出新进程运行这个a.out
//    while (1) {
//    }
    
    endutxent(); //close utmpx database
    return 0;
}

void showInfo(struct utmpx whoInfo)
{
    //只打印登录状态的信息。
    if (whoInfo.ut_type == USER_PROCESS) {
         printf("%s\n%s\n%s\n%d\t%d\t%s\t%s\n",whoInfo.ut_user,whoInfo.ut_id,whoInfo.ut_line,whoInfo.ut_pid,whoInfo.ut_type, ctime
                (&whoInfo.ut_tv.tv_sec), whoInfo.ut_host);
    }
   
}