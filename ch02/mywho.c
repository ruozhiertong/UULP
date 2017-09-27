#include <stdio.h>
#include <utmpx.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#define UTMP_FILE   "/var/run/utmpx"

void showInfo(struct utmpx whoinfo);
int main()
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
    while ((whoInfo = getutxent()))
    {
        showInfo(*whoInfo);
    }
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