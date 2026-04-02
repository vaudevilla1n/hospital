#include "tui_form.h"

#include "util.h"

#include <ctype.h>

#define FORM_TEXT_BOX_LEN	32

struct form tui_add_patient_form;

struct form_entry tui_add_patient_form_entries[] = {
	{ FORM_ENTRY_TEXT, lit("first") },
	{ FORM_ENTRY_TEXT, lit("middle") },
	{ FORM_ENTRY_TEXT, lit("last") },
};

char form_input_buffer[FORM_TEXT_BOX_LEN];
ptrdiff_t form_input_buffer_len = 0;

static void init_form(struct form *form)
{
	form->x = TUI_PAGE_X * 1/2;
	form->y = TUI_PAGE_Y;

	form->space = TUI_SPACE_Y;

	form->state = FORM_IDLE;

	form->curr_entry = 0;
	form->nentries = countof(tui_add_patient_form_entries);
	form->entries = tui_add_patient_form_entries;
}

void init_forms(void)
{
	init_form(&tui_add_patient_form);
}

static void draw_text_entry(void)
{
	for (int i = 0; i < FORM_TEXT_BOX_LEN; i++)
		addch('_');
}

static void draw_form_entry(const struct form *form, const ptrdiff_t entry)
{
	const int y = form->y + (form->space * entry);
	const int x = form->x;

	if (entry == form->curr_entry) {
		attron(A_UNDERLINE);
		mvaddstr(y, x, form->entries[entry].label);
		attroff(A_UNDERLINE);
	} else {
		mvaddstr(y, x, form->entries[entry].label);
	}

	addstr(": ");

	draw_text_entry();
}

static void draw_form(const struct form *form)
{
	for (ptrdiff_t i = 0; i < form->nentries; i++)
		draw_form_entry(form, i);
}

static void form_select_next_option(struct form *form)
{
	const ptrdiff_t prev = form->curr_entry;
	form->curr_entry = (form->curr_entry + 1) % form->nentries;

	draw_form_entry(form, prev);
	draw_form_entry(form, form->curr_entry);
}

static void form_select_prev_option(struct form *form)
{
	const ptrdiff_t prev = form->curr_entry;

	if (form->curr_entry > 0)
		form->curr_entry--;
	else
		form->curr_entry = form->nentries - 1;

	draw_form_entry(form, prev);
	draw_form_entry(form, form->curr_entry);
}

static inline void move_entry_end(struct form *form) {
	const struct form_entry *entry = form->entries + form->curr_entry;

	const int label_off = form->x + entry->label_len + lengthof(": ");

	const int y = form->y + (form->curr_entry * form->space);
	const int x = label_off + form_input_buffer_len;

	move(y, x);
}

static void enter_read_mode(struct form *form)
{
	form->state = FORM_READ;
}

static void exit_read_mode(struct form *form)
{
	form->state = FORM_IDLE;

	if (form_input_buffer_len + 1 < FORM_TEXT_BOX_LEN) {
		move_entry_end(form);

		attroff(A_BOLD);
		addch('_');
	}

	form_input_buffer_len = 0;
}

static void form_read_iteration(struct form *form)
{
	if (form_input_buffer_len + 1 < FORM_TEXT_BOX_LEN) {
		move_entry_end(form);

		attron(A_BOLD);
		addch('_');
		attroff(A_BOLD);
	}

	const int c = getch();

	if (c == ESCAPE_KEY) {
		exit_read_mode(form);
	} else if (isalpha(c) && form_input_buffer_len + 1 <= FORM_TEXT_BOX_LEN) {
		move_entry_end(form);

		addch(c);

		form_input_buffer[form_input_buffer_len++] = c;
	}
}

static enum tui_state form_idle_iteration(struct form *form)
{
	enum tui_state state = TUI_NONE;

	switch (getch()) {
	case 's':
	case 'S':
	case 'j':
	case 'J':
	case KEY_DOWN:	form_select_next_option(form); break;

	case 'w':
	case 'W':
	case 'k':
	case 'K':
	case KEY_UP:	form_select_prev_option(form); break;
	
	case ENTER_KEY:	enter_read_mode(form); break;

	case 'q':	state = TUI_EXIT; break;

	case 'b':	state = TUI_ADD; break;

	case 'm':	state = TUI_MAIN_MENU; break;
	}

	return state;
}

static enum tui_state form_iteration(struct form *form)
{
	switch (form->state) {
	case FORM_READ:	form_read_iteration(form); return TUI_NONE;
	default:	return form_idle_iteration(form);
	}
}

enum tui_state add_patient_form_iteration(const bool update)
{
	if (update) {
		tui_draw_title("new patient information");
		draw_form(&tui_add_patient_form);
		tui_draw_border();
	}

	return form_iteration(&tui_add_patient_form);
}

