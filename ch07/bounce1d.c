//gcc bounce1d.c -l curse

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

#define MESSAGE	"hello"
#define BLANK	"     "

int row;
int col;
int dir;
int delay;
int ndelay;

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

void enable_kbd_signals()
{
	int fd_flags;
	fcntl(0, F_SETOWN , getpid());

	fd_flags = fcntl(0, F_GETFL);

	fcntl(0, F_SETFL, (fd_flags | O_ASYNC)); //设置非阻塞 （异步）。 好像对getchar 和 getch没什么作用，都是阻塞的。
}

void move_msg(int sig_no)
{
	signal(SIGALRM , move_msg);

	move(row, col);

	addstr(BLANK);

	col += dir;

	move(row, col);

	addstr(MESSAGE);

	refresh();

	if(dir == -1 && col <= 0)
		dir = -dir;
	else if(dir == 1 && col + strlen(MESSAGE) >= COLS)
		dir = -dir; 

}

int set_ticker(int n_msecs)
{
	struct itimerval new_timeset;

	long n_sec, n_usecs;

	n_sec = n_msecs /1000;
	n_usecs = (n_msecs % 1000)  * 1000L;

	//间隔
	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;

	//开始时间
	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL , &new_timeset , NULL);

}

void set_up()
{
	tty_mode(0);

//进入course的控制台。
	initscr();

//这是对原来控制台的作用。
//	set_cr_noecho_mode();
//要对course控制台起作用，用course库中的函数
	crmode();
	noecho();
	clear();
	enable_kbd_signals();

	row = 10;
	col = 0;
	dir = 1;
	delay = 200 ; //0.2seconds

	move(row, col);
	addstr(MESSAGE);
	refresh();

	signal(SIGALRM, move_msg);
	set_ticker(delay);
}

void wrap_up()
{
	set_ticker(0);
	tty_mode(1);
	endwin();
}

int main()
{

	int c;

	set_up();

/*  //for test
	set_cr_noecho_mode();
	while(1)
	{
		int ch = getchar();
		printf("%d %c\n",ch , ch );
	}
*/

	//f : fase
	//s : slow
	while(1)
	{
		ndelay = 0;
		c = getch(); //好像和enable_kbd_signals无关，都是阻塞的。
		if(c == 'Q') break;
		if(c == ' ') dir = -dir;
		if(c == 'f' && delay > 2) ndelay = delay/2;
		if(c == 's') ndelay = delay *2;

		printf("test\n");
		if(ndelay > 0)
			set_ticker(delay = ndelay);

	}

	wrap_up();
	return 0;
}