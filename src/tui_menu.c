#include "tui_menu.h"

#include "tui.h"
#include "util.h"

#include <ctype.h>
#include <curses.h>

#define BUTTON(text, state)	{ OPT_BUTTON, lit(text), { .button = (state) } }
#define FORM(type, text)	{ (type), lit(text), { .form = { 0, { 0 } } } }

struct option tui_main_menu_opts[] = {
	BUTTON("add information", TUI_ADD),
	BUTTON("view information", TUI_VIEW),
	BUTTON("edit information", TUI_EDIT),
	BUTTON("search", TUI_SEARCH),
	BUTTON("exit", TUI_EXIT),
};

struct option tui_add_menu_opts[] = {
	BUTTON("new patient record", TUI_ADD_PATIENT),
	BUTTON("new doctor's appointment", TUI_ADD),
	BUTTON("back", TUI_MAIN_MENU),
	BUTTON("exit", TUI_EXIT),
};

struct option tui_add_patient_menu_opts[] = {
	FORM(OPT_FORM_TEXT, "first"),
	FORM(OPT_FORM_TEXT, "middle"),
	FORM(OPT_FORM_TEXT, "last"),
	BUTTON("back", TUI_ADD),
	BUTTON("exit", TUI_EXIT),
};

struct menu tui_main_menu;
struct menu tui_add_menu;
struct menu tui_add_patient_menu;

static inline void init_menu(struct menu *m, const char *title, struct option *opts, const ptrdiff_t nopts)
{
	m->x = TUI_PAGE_X;
	m->y = TUI_PAGE_Y;

	m->space = TUI_SPACE_Y;

	m->title = title;

	m->curr_opt = 0;
	m->nopts = nopts;
	m->opts = opts;
}

void init_menus(void)
{
	init_menu(&tui_main_menu,
			"Hospital Management System",
			tui_main_menu_opts, countof(tui_main_menu_opts));

	init_menu(&tui_add_menu,
			"add information",
			tui_add_menu_opts, countof(tui_add_menu_opts));

	init_menu(&tui_add_patient_menu,
			"new patient information",
			tui_add_patient_menu_opts, countof(tui_add_patient_menu_opts));
}

static void draw_text_form(const struct option *opt)
{
	addstr(": ");

	addnstr(opt->as.form.buf, opt->as.form.buflen);
	for (int i = opt->as.form.buflen; i < FORM_TEXT_LEN; i++)
		addch('_');
}

static void draw_option(const struct menu *menu, const ptrdiff_t i)
{
	const struct option *opt = menu->opts + i;

	const int y = menu->y + (menu->space * i);
	const int x = menu->x;

	if (i == menu->curr_opt) {
		attron(A_UNDERLINE);
		mvaddnstr(y, x, opt->text, opt->textlen);
		attroff(A_UNDERLINE);
	} else {
		mvaddnstr(y, x, opt->text, opt->textlen);
	}

	switch (opt->type) {
	case OPT_FORM_TEXT:
	case OPT_FORM_NUMBER: {
		draw_text_form(opt);
	} break;

	default:		break;
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

static enum tui_state read_form_input(const struct menu *menu, struct option *opt)
{
	const int text_off = menu->x + opt->textlen + lengthof(": ");
	const int y = menu->y + (menu->curr_opt * menu->space);

	struct form *form = &opt->as.form;

	for (;;) {
		const int x = text_off + form->buflen;

		draw_input_cursor(x, y);

		const int c = getch();

		if (c == ESCAPE_KEY || c == ENTER_KEY){ 
			mvaddch(y, x, '_');
			break;
		}

		switch (c) {
		case '-':
		case DELETE_KEY:
		case BACKSPACE_KEY: {
			if (form->buflen > 0) {
				mvaddch(y, x, '_');
				form->buflen--;
			}
		} break;

		default: {
			if (isalpha(c) && form->buflen + 1 <= FORM_TEXT_LEN) {
				mvaddch(y, x, c);
				form->buf[form->buflen++] = c;
			}
		}
		}
	}

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
