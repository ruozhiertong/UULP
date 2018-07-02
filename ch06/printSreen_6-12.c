#include "stdio.h"
#include "termios.h"
#include "fcntl.h"
#include "string.h"
#include "ctype.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/ioctl.h"



//ref:https://blog.csdn.net/u014507230/article/details/45314215

void tty_mode(int how);
void pr_winsize();
void set_cr_noecho_mode();
void set_nodelay_mode();

int ch = 'A';
int main(int argc, char const *argv[])
{
     int sig;
     tty_mode(0);
     set_cr_noecho_mode();
     set_nodelay_mode();
     pr_winsize();
     while(1){
          if((sig = getchar()) == 'Q'){
               tty_mode(1);
               system("clear");
               exit(0);
          }
          else if(sig != -1){
               ch = 'A';
               pr_winsize();
               continue;
          }
          signal(SIGWINCH, pr_winsize);
     }
     return 0;
}
void pr_winsize(){
     struct winsize size;

     if(ioctl(0, TIOCGWINSZ, (char *)&size) < 0)
     {
          perror("TIOCGWINSZ error");
          exit(1);
     }
     printf("row = %d, col = %d\n", size.ws_row, size.ws_col);
     for (int i = 0; i < size.ws_row; ++i)
     {
          for (int j = 0; j < size.ws_col; ++j)
          {
               printf("%c", ch);
          }
          //if(i < (size.ws_row -1))
          //     printf("\n");
     }
     fflush(stdout);
     ch++;
}

void tty_mode(int how){
     static struct termios orignal_mode;
     static int orignal_mode_flags;
     static int stored;

     if(how == 0){
          tcgetattr(0, &orignal_mode);
          orignal_mode_flags = fcntl(0, F_GETFL);
          stored = 1;
     }
     else if(stored){
          tcsetattr(0, TCSANOW, &orignal_mode);
          fcntl(0, F_SETFL, orignal_mode_flags);
     }
}

void set_cr_noecho_mode(){
     struct termios ttystate;

     tcgetattr(0, &ttystate);
     ttystate.c_lflag &= ~ICANON;
     ttystate.c_lflag &= ~ECHO;
     ttystate.c_cc[VMIN] = 1;
     tcsetattr(0, TCSANOW, &ttystate);
}

void set_nodelay_mode(){
     int termflags;

     termflags = fcntl(0, F_GETFL);
     termflags |= O_NDELAY;
     fcntl(0, F_SETFL, termflags);
}