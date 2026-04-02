#pragma once

#include "tui.h"

#include <stddef.h>

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

enum tui_state main_menu_iteration(const bool update);
enum tui_state add_menu_iteration(const bool update);
