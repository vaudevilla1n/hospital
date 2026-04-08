#include "tui_menu.h"

#include "tui.h"
#include "util.h"

#include <ctype.h>
#include <curses.h>

#define TUI_TITLE_X	(COLS * 1/10)
#define TUI_TITLE_Y	(LINES * 1/10)

#define TUI_PAGE_X	(COLS * 3/8)
#define TUI_PAGE_Y	(LINES * 3/10)

#define TUI_FORM_X	(TUI_PAGE_X * 1/2)

#define TUI_SPACE	4

#define SPACER(start, n)	((start) + (TUI_SPACE * (n)))

struct menu tui_menus[TUI_TOTAL_STATES];

static inline struct option *new_option(struct menu *menu, const ptrdiff_t textlen, const char *text) 
{
	struct option *opt = menu->opts + menu->nopts++;

	opt->text = text;
	opt->textlen = textlen;

	return opt;
}

static inline void title(struct menu *menu, const ptrdiff_t textlen, const char *text) 
{
	struct option *opt = new_option(menu, textlen, text);

	opt->type = OPT_TITLE;
	opt->x = TUI_TITLE_X;
	opt->y = TUI_TITLE_Y + (menu->nopts - 1) * TUI_SPACE;
}

static inline void button(struct menu *menu, const ptrdiff_t textlen, const char *text,
		const enum tui_state state) 
{
	struct option *opt = new_option(menu, textlen, text);

	opt->type = OPT_BUTTON;
	opt->as.button = state;
	opt->x = TUI_PAGE_X;
	opt->y = menu->y + (menu->nopts - 1) * TUI_SPACE;
}

static inline void center_button(struct menu *menu, const ptrdiff_t textlen, const char *text,
		const enum tui_state state) 
{
	struct option *opt = new_option(menu, textlen, text);

	opt->type = OPT_BUTTON;
	opt->as.button = state;
	opt->x = TUI_FORM_X + FORM_INPUT_MAX * 1/2;
	opt->y = menu->y + (menu->nopts - 1) * TUI_SPACE;
}

static inline void form(struct menu *menu, const ptrdiff_t textlen, const char *text,
		const enum option_type type) 
{
	struct option *opt = new_option(menu, textlen, text);

	opt->type = type;
	switch (type) {
	case OPT_FORM_TEXT:	opt->as.form.bufcap = FORM_TEXT_LEN; break;
	case OPT_FORM_DATE:	opt->as.form.bufcap = FORM_DATE_LEN; break;
	case OPT_FORM_ROOM:	opt->as.form.bufcap = FORM_ROOM_LEN; break;

	default:	__builtin_unreachable();
	}

	opt->x = TUI_FORM_X;
	opt->y = menu->y + (menu->nopts - 1) * TUI_SPACE;
}

static inline void init_menu(struct menu *menu, const int y, const ptrdiff_t start_opt)
{
	menu->y = y;
	menu->curr_opt = start_opt;
}

static inline void init_main_menu(void)
{
	struct menu *main_menu = &tui_menus[TUI_MAIN_MENU];
	init_menu(main_menu, TUI_PAGE_Y, 1);

	title(main_menu, lit("Hospital Management System"));
	button(main_menu, lit("add information"), TUI_ADD);
	button(main_menu, lit("view information"), TUI_MAIN_MENU);
	button(main_menu, lit("edit information"), TUI_MAIN_MENU);
	button(main_menu, lit("search"), TUI_SEARCH);
	button(main_menu, lit("exit"), TUI_EXIT);
}

static inline void init_add_menu(void)
{
	struct menu *add_menu = &tui_menus[TUI_ADD];
	init_menu(add_menu, TUI_PAGE_Y, 1);

	title(add_menu, lit("add information"));
	button(add_menu, lit("new patient record"), TUI_ADD_PATIENT);
	button(add_menu, lit("new doctor's appointment"), TUI_ADD_APPOINTMENT);
	button(add_menu, lit("back"), TUI_MAIN_MENU);
	button(add_menu, lit("exit"), TUI_EXIT);
}

static inline void init_add_patient_menu(void)
{
	struct menu *add_patient_menu = &tui_menus[TUI_ADD_PATIENT];
	init_menu(add_patient_menu, TUI_PAGE_Y, 1);

	title(add_patient_menu, lit("new patient"));
	form(add_patient_menu, lit("first"), OPT_FORM_TEXT);
	form(add_patient_menu, lit("last"), OPT_FORM_TEXT);
	form(add_patient_menu, lit("DOB"), OPT_FORM_DATE);
	center_button(add_patient_menu, lit("save"), TUI_ADD_SAVE);
	center_button(add_patient_menu, lit("back"), TUI_ADD);
}

static inline void init_add_appointment_menu(void)
{
	struct menu *add_appointment_menu = &tui_menus[TUI_ADD_APPOINTMENT];
	init_menu(add_appointment_menu, TUI_TITLE_Y, 1);

	title(add_appointment_menu, lit("doctor's information"));
	form(add_appointment_menu, lit("first"), OPT_FORM_TEXT);
	form(add_appointment_menu, lit("last"), OPT_FORM_TEXT);
	form(add_appointment_menu, lit("room"), OPT_FORM_ROOM);
	form(add_appointment_menu, lit("date"), OPT_FORM_DATE);

	title(add_appointment_menu, lit("patient's information"));
	form(add_appointment_menu, lit("first"), OPT_FORM_TEXT);
	form(add_appointment_menu, lit("last"), OPT_FORM_TEXT);

	center_button(add_appointment_menu, lit("save"), TUI_ADD_SAVE);
	center_button(add_appointment_menu, lit("back"), TUI_ADD);
}

static inline void init_search_menu(void)
{
	struct menu *search_menu = &tui_menus[TUI_SEARCH];
	init_menu(search_menu, TUI_PAGE_Y, 1);

	title(search_menu, lit("search information"));
	form(search_menu, lit("search"), OPT_FORM_TEXT);

	center_button(search_menu, lit("back"), TUI_MAIN_MENU);
	center_button(search_menu, lit("exit"), TUI_EXIT);
}

void init_menus(void)
{
	init_main_menu();

	init_add_menu();
	init_add_patient_menu();
	init_add_appointment_menu();

	init_search_menu();
}

static void draw_form(const struct option *opt)
{
	addstr(": ");
	addnstr(opt->as.form.buf, opt->as.form.buflen);

	for (int i = opt->as.form.buflen; i < opt->as.form.bufcap; i++)
		addch('_');
}

bool is_form(const struct option *opt)
{
	return opt->type >= OPT_FORM_TEXT;
}

static void draw_option(const struct menu *menu, const ptrdiff_t i)
{
	const struct option *opt = menu->opts + i;

	if (i == menu->curr_opt) {
		attron(A_UNDERLINE);
		mvaddnstr(opt->y, opt->x, opt->text, opt->textlen);
		attroff(A_UNDERLINE);
	} else {
		mvaddnstr(opt->y, opt->x, opt->text, opt->textlen);
	}

	if (is_form(opt))
		draw_form(opt);
}

static inline void draw_border(void)
{
	box(tui_window, ACS_VLINE, ACS_HLINE);
}

static void draw_menu(const struct menu *menu)
{
	for (ptrdiff_t i = 0; i < menu->nopts; i++)
		draw_option(menu, i);

	draw_border();
}

static inline bool selectable(const struct menu *menu) {
	return menu->opts[menu->curr_opt].type != OPT_TITLE;
}

static void menu_cycle_next_option(struct menu *menu)
{
	const ptrdiff_t prev = menu->curr_opt;

	do {
		menu->curr_opt = (menu->curr_opt + 1) % menu->nopts;
	} while (!selectable(menu));

	draw_option(menu, prev);
	draw_option(menu, menu->curr_opt);
}

static void menu_cycle_prev_option(struct menu *menu)
{
	const ptrdiff_t prev = menu->curr_opt;

	do {
		menu->curr_opt = (menu->curr_opt > 0) ? menu->curr_opt - 1 : menu->nopts - 1;
	} while (!selectable(menu));

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
	if (form->buflen >= form->bufcap)
		return;

	bool append = false;

	switch (type) {
	case OPT_FORM_TEXT:
		append = (isalpha(c));
		break;

	case OPT_FORM_ROOM:
	case OPT_FORM_DATE:
		append = (isdigit(c));
		break;

	default:	__builtin_unreachable();
	}

	if (!append)
		return;

	mvaddch(y, x, c);
	form->buf[form->buflen++] = c;
}

static inline void clear_cursor(const int x, const int y, const struct form *form)
{
	mvaddch(y, x, form->buflen == form->bufcap ? ' ' : '_');
}

static enum tui_state read_form_input(struct option *opt)
{
	const int text_off = opt->x + opt->textlen + lengthof(": ");
	const int y = opt->y;

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
				clear_cursor(x, y, form);
				form->buflen--;
			}
		} break;

		default:	append_input(opt->type, form, x, y, c);
		}
	}

	if (form->buflen <= form->bufcap)
		clear_cursor(x, y, form);

	return TUI_NONE;
}

static enum tui_state menu_select_option(struct menu *menu)
{
	struct option *opt = menu->opts + menu->curr_opt;
	return (opt->type == OPT_BUTTON) ? opt->as.button : read_form_input(opt);
}

enum tui_state menu_iteration(struct menu *menu, const bool update)
{
	if (update)
		draw_menu(menu);

	enum tui_state state = TUI_NONE;

	switch (getch()) {
	case 'm':
		state = TUI_MAIN_MENU;
		break;
	
	case 'q':
		state = TUI_EXIT;
		break;

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
