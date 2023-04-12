#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#define BLANK " "

char* ball;
int delay = 500, ndelay = 0;
int row, col, row_dir, col_dir;
int startrow, endrow, startcol, endcol;

void draw_boundary();
void move_ball(int signum);
int set_ticker(int n_msecs);

int main(int argc, char* argv[]) {
	row_dir = 1;
	col_dir = 1;
	ball	 = argv[1];
	row	 = atoi(argv[2]);
	col	 = atoi(argv[3]);
	startrow = atoi(argv[4]);
	endrow	 = atoi(argv[5]);
	startcol = atoi(argv[6]);
	endcol	 = atoi(argv[7]);

	initscr();
	crmode();
	noecho();
	clear();

	draw_boundary();

	move(row, col);
	addstr(ball);
	
	signal(SIGALRM, move_ball);
	set_ticker(delay);

	while(1) {
		ndelay = 0;
		char c = getch();

		if(c == 'q') col_dir /= 2;
		else if(c == 'w') row_dir /= 2;
		else if(c == 'e') col_dir *= 2;
		else if(c == 'r') row_dir *= 2;
		else if(c == 'Q') break;
	}

	endwin();
	return 0;
}

void draw_boundary() {
	move(startrow, startcol);

	for(int i = 0; i <= endcol - startcol; i++)
		addstr("*");

	for(int i = 0; i <= endrow - startrow; i++) {
		move(startrow + i, startcol);
		addstr("*");
		move(startrow + i, endcol);
		addstr("*");
	}

	move(endrow, startcol);
	for(int i = 0; i <= endcol - startcol; i++)
		addstr("*");
}

void move_ball(int signum) {
	signal(SIGALRM, move_ball);
	
	move(row, col);
	addstr(BLANK);
	row += row_dir;
	col += col_dir;

	if(row < startrow)
		row = startrow;
	else if(row < endrow)
		row = endrow;
	if(col < startcol)
		col = startcol;
	else if(col > endcol)
		col = endcol;

	move(row, col);
	if(startrow <= row && row <= endrow && startcol <= col && col <= endcol)
		addstr(ball);
	refresh();

	if(startrow >= row || row <= endrow) {
		ndelay = delay * 2;
		row_dir = -row_dir;
		set_ticker(ndelay);
		delay = ndelay;
	}

	if(startcol >= col || col <= endcol) {
		ndelay = delay / 2;
		col_dir = -col_dir;
		set_ticker(ndelay);
		delay = ndelay;
	}
}

int set_ticker(int n_msecs) {
	struct itimerval new_timeset;
	long n_sec, n_usecs;

	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;

	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;
	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
