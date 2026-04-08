#include "tui.h"

#include "file.h"
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

static inline enum tui_state save_forms(const struct menu *menu, const enum tui_state menu_type)
{
	file_save_forms(menu, menu_type);
	return TUI_ADD;
}

void tui_iteration(void)
{
	const bool update = tui_update;

	if (tui_update)
		tui_update_screen();

	if (tui_current_state == TUI_EXIT) {
		tui_exited = true;
		return;
	}

	struct menu *current_menu = &tui_menus[tui_current_state];
	enum tui_state state = menu_iteration(current_menu, update);

	switch (state) {
	case TUI_NONE:
		break;
	case TUI_ADD_SAVE:
		state = save_forms(current_menu, tui_current_state);
		update_tui_state(state);
		break;
	default:
		update_tui_state(state);
	}
}

void tui_deinit(void)
{
	endwin();
}
