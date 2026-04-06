#include "tui_menu.h"

#include "tui.h"
#include "util.h"

#include <ctype.h>
#include <curses.h>

#define BUTTON(space_x, space_y, text, state)	\
	{ (space_x), (space_y), OPT_BUTTON, lit(text), { .button = (state) } }

#define FORM(space_x, space_y, text, type)	\
	{ (space_x), (space_y), (type), lit(text), { .form = { 0, { 0 } } } }

struct option tui_main_menu_opts[] = {
	BUTTON(0, 0, "add information",		TUI_ADD),
	BUTTON(0, 0, "view information",	TUI_VIEW),
	BUTTON(0, 0, "edit information",	TUI_EDIT),
	BUTTON(0, 0, "search",			TUI_SEARCH),
	BUTTON(0, 0, "exit",			TUI_EXIT),
};

struct option tui_search_menu_opts[] = {
	FORM(0, 0, "search", 			OPT_FORM_TEXT),
	BUTTON(FORM_INPUT_MAX * 1/2, 0, "back",	TUI_MAIN_MENU),
	BUTTON(FORM_INPUT_MAX * 1/2, 0, "exit",	TUI_EXIT),
};

struct option tui_add_menu_opts[] = {
	BUTTON(0, 0, "new patient record",		TUI_ADD_PATIENT),
	BUTTON(0, 0, "new doctor's appointment",	TUI_ADD),
	BUTTON(0, 0, "back",				TUI_MAIN_MENU),
	BUTTON(0, 0, "exit",				TUI_EXIT),
};

struct option tui_add_patient_menu_opts[] = {
	FORM(0, 0, "first",		OPT_FORM_TEXT),
	FORM(0, 0, "middle",		OPT_FORM_TEXT),
	FORM(0, 0, "last",		OPT_FORM_TEXT),
	FORM(0, 0, "DOB",		OPT_FORM_DATE),
	BUTTON(TUI_SPACE, 0, "back",	TUI_ADD),
};

struct menu tui_menus[TUI_TOTAL_STATES];

static inline void init_menu(struct menu *m, const char *title,
		const int x, const int y, const int space,
		struct option *opts, const ptrdiff_t nopts)
{
	m->x = x;
	m->y = y;

	m->space = space;

	m->title = title;

	m->curr_opt = 0;
	m->nopts = nopts;
	m->opts = opts;
}

void init_menus(void)
{
	init_menu(&tui_menus[TUI_MAIN_MENU],
			"Hospital Management System",
			TUI_PAGE_X, TUI_PAGE_Y, TUI_SPACE,
			tui_main_menu_opts, countof(tui_main_menu_opts));

	init_menu(&tui_menus[TUI_ADD],
			"add information",
			TUI_PAGE_X, TUI_PAGE_Y, TUI_SPACE,
			tui_add_menu_opts, countof(tui_add_menu_opts));

	init_menu(&tui_menus[TUI_ADD_PATIENT],
			"new patient information",
			TUI_PAGE_X * 1/2, TUI_PAGE_Y, TUI_SPACE * 1/2,
			tui_add_patient_menu_opts, countof(tui_add_patient_menu_opts));

	init_menu(&tui_menus[TUI_SEARCH],
			"search",
			TUI_PAGE_X * 1/2, TUI_PAGE_Y, TUI_SPACE,
			tui_search_menu_opts, countof(tui_search_menu_opts));
}

static void draw_form(const struct option *opt)
{
	int len = 0;

	switch (opt->type) {
	case OPT_FORM_TEXT:	len = FORM_TEXT_LEN; break;
	case OPT_FORM_DATE:	len = FORM_DATE_LEN; break;

	default:		len = FORM_INPUT_MAX;
	}

	addnstr(opt->as.form.buf, opt->as.form.buflen);

	for (int i = opt->as.form.buflen; i < len; i++)
		addch('_');
}

static void draw_option(const struct menu *menu, const ptrdiff_t i)
{
	const struct option *opt = menu->opts + i;

	const int y = menu->y + (menu->space * i) + opt->y;
	const int x = menu->x + opt->x;

	if (i == menu->curr_opt) {
		attron(A_UNDERLINE);
		mvaddnstr(y, x, opt->text, opt->textlen);
		attroff(A_UNDERLINE);
	} else {
		mvaddnstr(y, x, opt->text, opt->textlen);
	}

	if (opt->type != OPT_BUTTON) {
		addstr(": ");
		draw_form(opt);
	}
}

static void draw_menu(const struct menu *menu)
{
	tui_draw_title(menu->title);

	for (ptrdiff_t i = 0; i < menu->nopts; i++)
		draw_option(menu, i);

	tui_draw_border();
}

static void menu_cycle_next_option(struct menu *menu)
{
	const ptrdiff_t prev = menu->curr_opt;
	menu->curr_opt = (menu->curr_opt + 1) % menu->nopts;

	draw_option(menu, prev);
	draw_option(menu, menu->curr_opt);
}

static void menu_cycle_prev_option(struct menu *menu)
{
	const ptrdiff_t prev = menu->curr_opt;

	if (menu->curr_opt > 0)
		menu->curr_opt--;
	else
		menu->curr_opt = menu->nopts - 1;

	draw_option(menu, prev);
	draw_option(menu, menu->curr_opt);
}

static void draw_input_cursor(const int x, const int y)
{
	attron(A_BOLD);
	mvaddch(y, x, 'I');
	attroff(A_BOLD);
}

static inline void append_input(const enum option_type type, struct form *form,
		const int x, const int y, const char c)
{
	bool append = false;

	switch (type) {
	case OPT_FORM_TEXT:	append = (isalpha(c) && form->buflen + 1 <= FORM_TEXT_LEN); break;
	case OPT_FORM_DATE:	append = (isdigit(c) && form->buflen + 1 <= FORM_DATE_LEN); break;

	default:	__builtin_unreachable();
	}

	if (append) {
		mvaddch(y, x, c);
		form->buf[form->buflen++] = c;
	}
}

static enum tui_state read_form_input(const struct menu *menu, struct option *opt)
{
	int maxlen = 0;

	switch (opt->type) {
	case OPT_FORM_TEXT:	maxlen = FORM_TEXT_LEN; break;
	case OPT_FORM_DATE:	maxlen = FORM_DATE_LEN; break;

	default:	__builtin_unreachable();
	}

	const int text_off = menu->x + + opt->x + opt->textlen + lengthof(": ");
	const int y = menu->y + opt->y + (menu->curr_opt * menu->space);
	int x;

	struct form *form = &opt->as.form;

	for (;;) {
		x = text_off + form->buflen;

		draw_input_cursor(x, y);

		const int c = getch();

		if (c == ESCAPE_KEY || c == ENTER_KEY)
			break;

		switch (c) {
		case '-':
		case DELETE_KEY:
		case BACKSPACE_KEY: {
			if (form->buflen > 0) {
				mvaddch(y, x, form->buflen == maxlen ? ' ' : '_');
				form->buflen--;
			}
		} break;

		default:	append_input(opt->type, form, x, y, c);
		}
	}

	if (form->buflen < maxlen)
		mvaddch(y, x, form->buflen == maxlen ? ' ' : '_');

	return TUI_NONE;
}

static enum tui_state menu_select_option(struct menu *menu)
{
	struct option *opt = menu->opts + menu->curr_opt;

	switch (opt->type) {
	case OPT_BUTTON:	return opt->as.button;
	default:		return read_form_input(menu, opt);
	}
}

enum tui_state menu_iteration(struct menu *menu, const bool update)
{
	if (update)
		draw_menu(menu);

	enum tui_state state = TUI_NONE;

	switch (getch()) {
	case KEY_DOWN:
	case 's':
	case 'S':
	case 'j':
	case 'J':
		menu_cycle_next_option(menu);
		break;

	case 'w':
	case 'W':
	case 'k':
	case 'K':
	case KEY_UP:
		menu_cycle_prev_option(menu);
		break;

	case ENTER_KEY:
		state = menu_select_option(menu);
		break;
	}

	return state;
}
