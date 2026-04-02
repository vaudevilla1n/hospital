#include "tui_form.h"

#include "util.h"

#define FORM_TEXT_BOX_LEN	32

struct form tui_add_patient_form;

struct form_entry tui_add_patient_form_entries[] = {
	{ FORM_TEXT, "first" },
	{ FORM_TEXT, "middle" },
	{ FORM_TEXT, "last" },
};

static void init_form(struct form *form)
{
	form->x = TUI_PAGE_X * 1/2;
	form->y = TUI_PAGE_Y;

	form->space = TUI_SPACE_Y;

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
	const int y = form->y + (TUI_SPACE_Y * entry);
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

void draw_form(const struct form *form)
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

enum tui_state form_iteration(struct form *form)
{
	enum tui_state state = TUI_NONE;

	switch (getch()) {
	case KEY_DOWN:
	case 's':
	case 'S':
	case 'j':
	case 'J':
		form_select_next_option(form);
		break;

	case 'w':
	case 'W':
	case 'k':
	case 'K':
	case KEY_UP:
		form_select_prev_option(form);
		break;

	case 'q':	state = TUI_EXIT; break;
	case 'm':	state = TUI_MAIN_MENU; break;
	case 'b':	state = TUI_ADD; break;
	}

	return state;
}
