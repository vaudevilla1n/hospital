#include "tui.h"

#include "util.h"
#include "tui_menu.h"

#include <stddef.h>
#include <stdlib.h>
#include <locale.h>
#include <curses.h>

bool tui_exited = false;
bool tui_update = true;

WINDOW *tui_window;

enum tui_state tui_current_state = TUI_MAIN_MENU;

static inline void curses_assert(const int ret, const char *msg)
{
	if (ret == ERR) {
		tui_deinit();
		fprintf(stderr, "curses: %s\n", msg);
		exit(1);
	}
}

void tui_init(void)
{
	setlocale(LC_ALL, "");

	tui_window = initscr();

	curses_assert(cbreak(), "couldn't set cbreak mode");
	curses_assert(noecho(), "couldn't set no-echo mode");
	curses_assert(curs_set(0), "couldn't set cursor visibility mode");
	curses_assert(keypad(tui_window, TRUE), "couldn't set keypad mode");

	init_menus();
}

void tui_draw_border(void)
{
	box(tui_window, ACS_VLINE, ACS_HLINE);
}

void tui_draw_title(const char *title)
{
	mvaddstr(TUI_TITLE_Y, TUI_TITLE_X, title);
}

static void tui_update_screen(void)
{
	tui_update = false;
	clear();
}

static void update_tui_state(const enum tui_state state)
{
	tui_current_state = state;
	tui_update = true;
}

void tui_iteration(void)
{
	const bool update = tui_update;

	if (tui_update)
		tui_update_screen();

	enum tui_state state = TUI_NONE;

	switch (tui_current_state) {
	case TUI_EXIT:		tui_exited = true; break;

	case TUI_ADD:		state = menu_iteration(&tui_add_menu,  update); break;
	case TUI_ADD_PATIENT:	state = menu_iteration(&tui_add_patient_menu, update); break;

	default:		state = menu_iteration(&tui_main_menu, update); break;
	}

	if (state != TUI_NONE)
		update_tui_state(state);
}

void tui_deinit(void)
{
	endwin();
}
