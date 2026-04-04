#pragma once

#include "tui.h"

#include <stddef.h>

#define FORM_INPUT_MAX	32

#define FORM_TEXT_LEN	32
#define FORM_DATE_LEN	8

struct form {
	ptrdiff_t buflen;
	char buf[FORM_INPUT_MAX];
};

enum option_type {
	OPT_BUTTON,
	OPT_FORM_TEXT,
	OPT_FORM_NUMBER,
};

struct option {
	enum option_type type;	

	ptrdiff_t textlen;
	char *text;

	union {
		enum tui_state button;
		struct form form;
	} as;
};

struct menu {
	int x;
	int y;

	int space;

	const char *title;

	ptrdiff_t curr_opt;

	ptrdiff_t nopts;
	struct option *opts;
};

extern struct menu tui_main_menu;
extern struct menu tui_add_menu;
extern struct menu tui_add_patient_menu;

void init_menus(void);

enum tui_state menu_iteration(struct menu *menu, const bool update);
