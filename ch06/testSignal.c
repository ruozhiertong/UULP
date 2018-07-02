#include <stdio.h>
#include <signal.h>
#include "unistd.h"
#include <sys/wait.h>




//#define MYSIG_MSG SIGUSR1+20
#define MYSIG_MSG  38


//macos 下似乎 不支持__SIGRTMIN 也不支持信号的自定义，只能用系统中定义的信号。
//自定义信号
//#define SIG_MY_DEFINE_TEST    __SIGRTMIN+10
#define SIG_MY_DEFINE_TEST    42

void    sigactionProcess(int nsig)
{
    //================================================================
    //TODO:ADD YOU CODE
    
    printf("son process sigactionProcess end !pid is:%d\n", getpid()) ;
    //================================================================
    exit(0);
}
//信号处理函数注册
void    sigactionReg(void)
{
    struct sigaction act,oldact;
    
    act.sa_handler  = sigactionProcess;
    act.sa_flags = 0;
    
    //sigaction(SIGINT,&act,&oldact);
    if(sigaction(SIG_MY_DEFINE_TEST,&act,&oldact) == -1)
    {
        printf("sigaction erro\n");
    }
}





void handle_signal(int signal_no)
{
	printf("receive signal %d\n", signal_no);
	if(signal_no == SIGUSR1)
	{
		printf("receive SIGUSR1 %d\n", signal_no);
	}
    
    if(signal_no == MYSIG_MSG)
    {
        printf("receive MYSIG_MSG %d\n", signal_no);
        
    }

}
int main()
{


	if(signal(SIGUSR1 , handle_signal) == SIG_ERR)
	{
		printf("SIGUSR1 erro\n");
		return -1;
	}

	if(signal(SIGINT , handle_signal))
	{
		printf("SIGINT erro\n");
		return -1;
	}
    
    //自定义信号。mac下自定信号不能用？？TODO.
    if(signal(MYSIG_MSG , handle_signal))
    {
       printf("MYSIG_MSG erro\n");
       //return -1;
    }
    
    
	/*
	//无法对SIGKILL进行捕获， 所以也无法进行忽略。
	if(signal(SIGKILL , SIG_IGN))
	{
		printf("SIGKILL erro\n");
		return -1;
	}
	*/

	/*
	//同上
	if(signal(SIGSTOP , handle_signal))
	{
		printf("SIGSTOP erro\n");
		return -1;
	}
	*/
    
    
    int ret ;
    pid_t pid;/*pid 进程id号*/
    
    pid=fork();/*创建一个新进程*/
    
    if(pid==0) /*返回0为子进程*/
    {
        printf("son get Return pid is %d\n",pid);
        printf("This is son process!  pid is:%d\n",getpid());
        //信号处理函数注册
        sigactionReg();
        //raise(SIGSTOP) ;//子进程暂停
        while(1){}
        printf("son process end !pid is:%d\n", getpid()) ;
        exit(0) ;
    }
    else if(pid>0)/*返回大于0为父进程 且 此时pid 为子进程的进程id*/
    {
        printf("parent get Return pid is %d\n",pid);
        printf("This is parent process!  pid is:%d\n",getpid());
        //
        usleep(1000*10);
        //获取到pid子进程没有退出,指定WNOHANG不会阻塞，没有退出会返回0
        if ((waitpid(pid, NULL, WNOHANG)) == 0)
        {
            //if ((ret = kill(pid, SIGINT)) == 0)//向子进程发出SIGKILL信号
            if ((ret = kill(pid, SIG_MY_DEFINE_TEST)) == 0)//向子进程发出SIG_MY_DEFINE_TEST信号
            {
                printf("parent kill %d\n", pid) ;
            }
        }
        waitpid(pid,NULL,0);/*等待子进程退出*/
        exit(0) ;
    }
    else
    {
        perror("fork() error!");  
        exit(-1) ;  
    }
    
    

	while(1);


	return 0;
}