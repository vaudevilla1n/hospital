#include <stdio.h>
#include <locale.h>
#include <curses.h>

int main(void)
{
	initscr();

	setlocale(LC_ALL, "");

	endwin();
}
