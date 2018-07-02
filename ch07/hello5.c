#include <stdio.h>
#include <unistd.h> // for sleep
#include <curses.h>

#define LEFTEDGE	10
#define RIGHTEDGE	30
#define ROW			10


int main()
{
	char *message = "hello world";
	char *blank   = "		   ";
	int dir = +1 ;
	int pos = LEFTEDGE;
	initscr();

	clear();

	while(1)
	{
		move(ROW,pos);
		addstr(message);
		refresh();
		sleep(1); //second
		move(ROW, pos);
		addstr(blank);

		pos += dir;

		if(pos >= RIGHTEDGE)
			dir = -1;
		if(pos <= LEFTEDGE)
			dir = +1;


	}
	getch();
	endwin();
	return 0;
}