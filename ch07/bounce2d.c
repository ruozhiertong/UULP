//gcc bounce1d.c -l curse

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>

#define BOARD_SYM	'l'
#define DEF_SYMBOL	'O'
#define BLANK	' '
#define TOP_ROW	5
#define BOTTOM_ROW	20
#define LEFT_EDGE	10
#define RIGHT_EDGE	70
#define START_ROW	10
#define START_COL	10
#define TICKS_PER_SEC	50
#define X_TTM	5
#define Y_TTM	8


#define BOARD_WIDTH	4

typedef struct ppball
{
	int pos_x;
	int pos_y;
	int x_dir;
	int y_dir;
	int x_ttm;
	int y_ttm;
	int x_ttg;
	int y_ttg;

	char symbol;

} ppball;

typedef struct board
{
	int pos_x;
	int pos_y;
	int width;
	char symbol;
}board;



ppball the_ball;

board the_board;

int game_over = 0;

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


int bounce_or_lose(ppball *bp)
{

	if(bp->pos_x >= RIGHT_EDGE)
	{
		if(bp->pos_y < the_board.pos_y || bp->pos_y > the_board.pos_y + BOARD_WIDTH)
			return -1; //lose game
		else
		{
			bp->x_dir = -1;
		}
	}
	else if(bp->pos_x <= LEFT_EDGE)
	{
		bp->x_dir = 1;
	}

	if(bp->pos_y <= TOP_ROW)
	{
		bp->y_dir = 1;
	}
	else if(bp->pos_y >= BOTTOM_ROW)
	{
		bp->y_dir = -1;
	}
	return 0;
}

void ball_move(int sig_no)
{
	void wrap_up();
	signal(SIGALRM , SIG_IGN);
	
	int cur_x, cur_y, moved;

	cur_x = the_ball.pos_x;
	cur_y = the_ball.pos_y;
	moved = 0;

	if(the_ball.y_ttm > 0 && the_ball.y_ttg-- == 1)
	{
		the_ball.pos_y += the_ball.y_dir;
		the_ball.y_ttg = the_ball.y_ttm;
		moved = 1;
	}

	if(the_ball.x_ttm > 0 && the_ball.x_ttg-- == 1)
	{
		the_ball.pos_x += the_ball.x_dir;
		the_ball.x_ttg = the_ball.x_ttm;
		moved = 1;

	}

	if(moved)
	{
		mvaddch(cur_y , cur_x, BLANK);
		mvaddch(the_ball.pos_y , the_ball.pos_x , the_ball.symbol);
		move(LINES -1, COLS -1);
		refresh();

		if(bounce_or_lose(&the_ball) == -1) //game over
		{
			game_over = 1;
			wrap_up();
			kill(getpid(), 9);
		}
	}

	signal(SIGALRM , ball_move);

}

int move_boad(int dir)
{
	if(dir < 0 && the_board.pos_y <= TOP_ROW)
		return -1;
	if(dir > 0 && the_board.pos_y + BOARD_WIDTH >= BOTTOM_ROW)
		return -1;


	for(int i = 0; i < BOARD_WIDTH ; i++)
	{
		mvaddch(the_board.pos_y + i, the_board.pos_x, BLANK);
	}

	the_board.pos_y += dir;

	for(int i = 0; i < BOARD_WIDTH ; i++)
	{
		mvaddch(the_board.pos_y + i, the_board.pos_x, the_board.symbol);
	}

	move(LINES -1, COLS -1);

	refresh();
	return 0;
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

	initscr();

	set_cr_noecho_mode();
	clear();

	the_ball.pos_y = START_COL;
	the_ball.pos_x = START_ROW;

	the_ball.x_ttg = the_ball.x_ttm = X_TTM;
	the_ball.y_ttg = the_ball.y_ttm = Y_TTM;

	the_ball.x_dir = 1;
	the_ball.y_dir = 1;

	the_ball.symbol = DEF_SYMBOL;

	the_board.pos_x = RIGHT_EDGE + 1;
	the_board.pos_y = TOP_ROW + (BOTTOM_ROW - TOP_ROW)/2;

	the_board.width = BOARD_WIDTH;

	the_board.symbol = BOARD_SYM;

	for(int i = 0; i < BOARD_WIDTH ; i++)
	{
		mvaddch(the_board.pos_y + i, the_board.pos_x , the_board.symbol);
	}


	//move(the_ball.pos_x, the_ball.pos_y);
	//addstr(the_ball.symbol);
	mvaddch(the_ball.pos_y, the_ball.pos_x, the_ball.symbol);
	refresh();

	signal(SIGINT, SIG_IGN);
	signal(SIGALRM, ball_move);
	set_ticker(1000/TICKS_PER_SEC);
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

	//f : fase
	//s : slow
	while(game_over != 1 && (c = getchar())!= 'Q')
	{
		//if(c == ' ') dir = -dir;
		if(c == 'f') the_ball.x_ttm--;
		if(c == 'F') the_ball.y_ttm--;

		if(c == 's') the_ball.x_ttm++;
		if(c == 'S') the_ball.y_ttm++;

		if(c == 'j') move_boad(1);
		if(c == 'k') move_boad(-1);
		//if(ndelay > 0)
		//	set_ticker(delay = ndelay);

	}

	wrap_up();
	return 0;
}