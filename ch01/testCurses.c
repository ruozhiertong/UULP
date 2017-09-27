#include <stdio.h>
#include "curses.h"
#include <unistd.h>
#include <termios.h>
#include <assert.h>
int main()
{
	char c = '\0';
	printf("please input a char\n");
	// initscr();
	// c = getch();
	// endwin();
	c = instant_get();

	printf("your input is %c\n",c);
	return 0;
}

int instant_get()
{
    int c = 0;
    int res = -1;
    struct termios org_opts, new_opts;
    
    res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);
    memcpy(&new_opts, &org_opts, sizeof(org_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    
    c = getchar();
    
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    
    return c;
}
