#pragma once

#include <stddef.h>

enum tui_state {
	TUI_MAIN_MENU,
	TUI_ADD,
	TUI_VIEW,
	TUI_SEARCH,
	TUI_EDIT,
	TUI_EXIT,
};

struct option {
	enum tui_state state;
	char *text;
};

struct menu {
	int x;
	int y;

	int space;

	ptrdiff_t curr_opt;

	ptrdiff_t nopts;
	const struct option *opts;
};

extern struct menu tui_main_menu;
extern struct menu tui_add_menu;

void init_menus(void);
void draw_menu(const struct menu *menu);

void menu_select_next_option(struct menu *menu);
void menu_select_prev_option(struct menu *menu);

enum tui_state menu_selected_option_state(const struct menu *menu);
