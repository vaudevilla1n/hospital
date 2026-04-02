#include "tui.h"

#include "util.h"
#include "tui_menu.h"
#include "tui_form.h"

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
	init_forms();
}

static void tui_update_screen(void)
{
	tui_update = false;
	clear();
}

static void draw_border(void)
{
	box(tui_window, ACS_VLINE, ACS_HLINE);
}

static void draw_title(const char *title)
{
	mvaddstr(TUI_TITLE_Y, TUI_TITLE_X, title);
}

static void draw_add_menu(void)
{
	if (tui_update) {
		tui_update_screen();

		draw_title("add information");
		draw_menu(&tui_add_menu);
		draw_border();
	}
}

static void update_tui_state(const enum tui_state state)
{
	tui_current_state = state;
	tui_update = true;
}

static void add_menu_iteration(void)
{
	draw_add_menu();

	const enum tui_state state = menu_iteration(&tui_add_menu);
	if (state != TUI_NONE)
		update_tui_state(state);
}

static inline void draw_main_menu(void)
{
	if (tui_update) {
		tui_update_screen();

		draw_title("Hospital Management System");
		draw_menu(&tui_main_menu);
		draw_border();
	}
}

static void main_menu_iteration(void)
{
	draw_main_menu();

	const enum tui_state state = menu_iteration(&tui_main_menu);
	if (state != TUI_NONE)
		update_tui_state(state);
}

static void add_patient_form_iteration(void)
{
	if (tui_update) {
		tui_update_screen();

		draw_title("new patient information");
		draw_form(&tui_add_patient_form);
		draw_border();
	}

	const enum tui_state state = form_iteration(&tui_add_patient_form);
	if (state != TUI_NONE)
		update_tui_state(state);
}

void tui_iteration(void)
{
	switch (tui_current_state) {
	case TUI_EXIT:	tui_exited = true; break;

	case TUI_ADD:		add_menu_iteration(); break;
	case TUI_ADD_PATIENT:	add_patient_form_iteration(); break;

	default:	main_menu_iteration(); break;
	}

}

void tui_deinit(void)
{
	endwin();
}
