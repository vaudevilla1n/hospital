#include "tui.h"
#include "util.h"
#include <stddef.h>
#include <stdlib.h>
#include <locale.h>
#include <curses.h>

bool tui_exited = false;
bool tui_update = true;

WINDOW *tui_window;

enum tui_state {
	TUI_MAIN_MENU,
	TUI_ADD,
	TUI_VIEW,
	TUI_SEARCH,
	TUI_EDIT,
	TUI_EXIT,
};

enum tui_state tui_current_state = TUI_MAIN_MENU;

#define ENTER_KEY	'\n'

#define TITLE	"Hospital Management System"	

#define TITLE_X ((COLS - lengthof(TITLE)) * 1/2)
#define TITLE_Y	(LINES * 1/10)

#define MENU_X		TITLE_X
#define MENU_Y		(LINES * 3/10)
#define MENU_Y_SPACE	4

struct option {
	char *text;
	enum tui_state state;
};

const struct option menu_options[] = {
	{ "add information", TUI_ADD },
	{ "view information", TUI_VIEW },
	{ "search", TUI_SEARCH },
	{ "edit information", TUI_EDIT },
	{ "exit", TUI_EXIT },
};
const ptrdiff_t total_menu_options = countof(menu_options); 

ptrdiff_t current_menu_option = 0;

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
}

static inline void clear_line(const int i)
{
	move(i, 0);
	clrtoeol();
}

static void draw_border(void)
{
	box(tui_window, ACS_VLINE, ACS_HLINE);
}

static void draw_title(void)
{
	mvinsstr(TITLE_Y, TITLE_X, TITLE);
}

static inline void draw_option(const ptrdiff_t i)
{
	mvinsstr(MENU_Y + (i * MENU_Y_SPACE), MENU_X, menu_options[i].text);
}

static void draw_selected_option(void)
{
	const struct option *opt = menu_options + current_menu_option;
	attron(A_UNDERLINE);
	mvinsstr(MENU_Y + (current_menu_option * MENU_Y_SPACE), MENU_X, opt->text);
	attroff(A_UNDERLINE);
}

static void draw_menu(void)
{
	for (ptrdiff_t i = 0; i < total_menu_options; i++) {
		if (i == current_menu_option)
			draw_selected_option();
		else
			draw_option(i);
	}
}

static void draw_newly_selected_option(const ptrdiff_t prev)
{
	clear_line(MENU_Y + (prev * MENU_Y_SPACE));
	draw_option(prev);

	clear_line(MENU_Y + (current_menu_option * MENU_Y_SPACE));
	draw_selected_option();

	draw_border();
}

static inline void select_next_option(void)
{
	const ptrdiff_t prev = current_menu_option;
	current_menu_option = (current_menu_option + 1) % total_menu_options;
	draw_newly_selected_option(prev);
}

static inline void select_prev_option(void)
{
	const ptrdiff_t prev = current_menu_option;

	if (current_menu_option > 0)
		current_menu_option--;
	else
		current_menu_option = total_menu_options - 1;

	draw_newly_selected_option(prev);
}

static inline void draw_main_menu(void)
{
	if (tui_update) {
		clear();
		draw_title();
		draw_menu();
		draw_border();

		tui_update = false;
	}
}

static void handle_selected_option(void)
{
	tui_current_state = menu_options[current_menu_option].state;
	tui_update = true;
}

static void main_menu_iteration(void)
{
	draw_main_menu();

	switch (getch()) {
	case 'q':	tui_exited = true; break;

	case 'j':
	case 'J':	select_next_option(); break;

	case 'k':
	case 'K':	select_prev_option(); break;

	case ENTER_KEY:	handle_selected_option(); break;
	}
}

void tui_iteration(void)
{
	switch (tui_current_state) {
	case TUI_EXIT:	tui_exited = true; break;
	default:	main_menu_iteration(); break;
	}

}

void tui_deinit(void)
{
	endwin();
}
