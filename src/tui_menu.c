#include "tui_menu.h"
#include "util.h"

#include <curses.h>

const struct option tui_main_menu_opts[] = {
	{ TUI_ADD, "add information" },
	{ TUI_VIEW, "view information" },
	{ TUI_SEARCH, "search" },
	{ TUI_EDIT, "edit information" },
	{ TUI_EXIT, "exit" },
};

const struct option tui_add_menu_opts[] = {
	{ TUI_ADD_PATIENT, "new patient record" },
	{ TUI_ADD, "new doctor's appointment" },
	{ TUI_MAIN_MENU, "back" },
	{ TUI_EXIT, "exit" },
};

struct menu tui_main_menu;
struct menu tui_add_menu;

static inline void init_menu(struct menu *m, const struct option *opts, const ptrdiff_t nopts)
{
	m->x = TUI_PAGE_X;
	m->y = TUI_PAGE_Y;

	m->space = TUI_SPACE_Y;

	m->curr_opt = 0;
	m->nopts = nopts;
	m->opts = opts;
}

void init_menus(void)
{
	init_menu(&tui_main_menu, tui_main_menu_opts, countof(tui_main_menu_opts));
	init_menu(&tui_add_menu, tui_add_menu_opts, countof(tui_add_menu_opts));
}

static inline void draw_option(const struct menu *menu, const ptrdiff_t i)
{
	mvaddstr(menu->y + (i * menu->space), menu->x, menu->opts[i].text);
}

static void draw_selected_option(const struct menu *menu)
{
	const struct option *opt = menu->opts + menu->curr_opt;
	attron(A_UNDERLINE);
	mvaddstr(menu->y + (menu->curr_opt * menu->space), menu->x, opt->text);
	attroff(A_UNDERLINE);
}

static void draw_menu(const struct menu *menu)
{
	for (ptrdiff_t i = 0; i < menu->nopts; i++) {
		if (i == menu->curr_opt)
			draw_selected_option(menu);
		else
			draw_option(menu, i);
	}
}

static inline void clear_text(const int x, const int y, const ptrdiff_t len)
{
	for (ptrdiff_t i = 0; i < len; i++)
		mvdelch(y, x + i);
}

static void menu_select_next_option(struct menu *menu)
{
	const ptrdiff_t prev = menu->curr_opt;
	menu->curr_opt = (menu->curr_opt + 1) % menu->nopts;

	draw_option(menu, prev);
	draw_selected_option(menu);
}

static void menu_select_prev_option(struct menu *menu)
{
	const ptrdiff_t prev = menu->curr_opt;

	if (menu->curr_opt > 0)
		menu->curr_opt--;
	else
		menu->curr_opt = menu->nopts - 1;

	draw_option(menu, prev);
	draw_selected_option(menu);
}

static enum tui_state menu_selected_option_state(const struct menu *menu)
{
	return menu->opts[menu->curr_opt].state;	
}

static enum tui_state menu_iteration(struct menu *menu)
{
	enum tui_state state = TUI_NONE;

	switch (getch()) {
	case KEY_DOWN:
	case 's':
	case 'S':
	case 'j':
	case 'J':
		menu_select_next_option(menu);
		break;

	case 'w':
	case 'W':
	case 'k':
	case 'K':
	case KEY_UP:
		menu_select_prev_option(menu);
		break;

	case ENTER_KEY:
		state = menu_selected_option_state(menu);
		break;
	}

	return state;
}

enum tui_state main_menu_iteration(const bool update)
{
	if (update) {
		tui_draw_title("Hospital Management System");
		draw_menu(&tui_main_menu);
		tui_draw_border();
	}

	return menu_iteration(&tui_main_menu);
}

enum tui_state add_menu_iteration(const bool update)
{
	if (update) {
		tui_draw_title("add information");
		draw_menu(&tui_add_menu);
		tui_draw_border();
	}

	return menu_iteration(&tui_add_menu);
}
