#pragma once

#include "tui.h"

#include <stddef.h>

#define FORM_TEXT_LEN	64
#define FORM_DATE_LEN	8
#define FORM_ROOM_LEN	3

#define FORM_INPUT_MAX	FORM_TEXT_LEN

/*
	a union of a 32 byte buffer and a 4 byte enum is kinda nuts
	however, i dont really care
*/
struct form {
	ptrdiff_t bufcap;
	ptrdiff_t buflen;
	char buf[FORM_INPUT_MAX];
};

#define MAX_OPTS	10

enum option_type {
	OPT_TITLE,
	OPT_BUTTON,
	OPT_FORM_TEXT,
	OPT_FORM_DATE,
	OPT_FORM_ROOM,
};

struct option {
	int x;
	int y;

	enum option_type type;	

	ptrdiff_t textlen;
	const char *text;

	union {
		enum tui_state button;
		struct form form;
	} as;
};

bool is_form(const struct option *opt);

struct menu {
	ptrdiff_t curr_opt;
	ptrdiff_t nopts;
	struct option opts[MAX_OPTS];
};

extern struct menu tui_menus[TUI_TOTAL_STATES];

void init_menus(void);

enum tui_state menu_iteration(struct menu *menu, const bool update);
