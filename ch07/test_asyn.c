#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include <termios.h>

int static count= 0;

void tty_mode(int how)
{
	static struct termios old_ttystate;
	static int stored = 0;
	if(how == 0) //save cur ttymode
	{
		tcgetattr(STDIN_FILENO, &old_ttystate);
		stored = 1;
	}
	else if(stored == 1) //restored ttymode
	{
		tcsetattr(STDIN_FILENO, TCSANOW , &old_ttystate);
		stored = 0;
	}

}

void set_cr_noecho_mode()
{
	struct termios ttystate;

	tcgetattr(STDIN_FILENO, &ttystate);

	ttystate.c_lflag &= ~ICANON;
	ttystate.c_lflag &= ~ECHO;
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void on_input(int sig_no)
{
	
	printf("count %d\n", count++);
	//int c = getch(); // 都是非阻塞的。
	//mvaddch(10, 10, c);
	//printf("%d %c\n", c, c);
	//if(c == 'Q' || c == EOF)
	//refresh();
}

void enable_kbd_signals()
{
	int fd_flags;
	fcntl(0, F_SETOWN , getpid());

	fd_flags = fcntl(0, F_GETFL);

	fcntl(0, F_SETFL, (fd_flags | O_ASYNC)); //设置非阻塞 （异步）。 好像对getchar 和 getch没什么作用，都是阻塞的。
}

int main()
{

	tty_mode(0);

	initscr();
	crmode(); //curses 中函数
	noecho();
	//set_cr_noecho_mode();
	enable_kbd_signals();
	clear();
	signal(SIGIO, on_input);//IO信号/键盘信号。 和异步还是非异步无关。

	//本想测试IO的异步，用getch 和getchar，本以为设置成非异步后，不用在getch getchar上等待阻塞，不过实验表明，
	//这里和设置异步还是非异步没关系，都会阻塞。应该哪里出问题了。正常情况，应该是设置成非异步之后，不用等待输入继续往下执行，
	//在同步情况下，等待输入，阻塞。
	while(1)
	{
		//getch() 是curses中的函数，要initscr才能使用
		int c = getch(); //getchar()是阻塞的。
		mvaddch(13,16, 'm');
		mvaddch(13, 13, c);
		refresh();
		//printf("in while  %c %d %d\n", c, c, count);
		//printf("hello\n");
		//pause();
	}
	endwin();

	tty_mode(1);
	return 0;
}