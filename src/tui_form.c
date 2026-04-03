#include "tui_form.h"

#include "util.h"

#include <ctype.h>

#define FORM_ENTRY(t, label)	{ (t), lit(label), 0, { 0 } }

struct form_entry tui_add_patient_form_entries[] = {
	FORM_ENTRY(FORM_ENTRY_TEXT, "first"),
	FORM_ENTRY(FORM_ENTRY_TEXT, "middle"),
	FORM_ENTRY(FORM_ENTRY_TEXT, "last"),
};

struct form tui_add_patient_form;

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

static void draw_text_entry(const struct form_entry *entry)
{
	addnstr(entry->buf, entry->buf_len);
	for (int i = entry->buf_len; i < FORM_ENTRY_TEXT_LEN; i++)
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

	draw_text_entry(form->entries + entry);
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

static void form_entry_append(const struct form *form, const struct form_entry *entry, const char c)
{
	const int label_off = form->x + entry->label_len + lengthof(": ");

	const int y = form->y + (form->curr_entry * form->space);
	const int x = label_off + entry->buf_len;

	mvaddch(y, x, c);
}

static void enter_read_mode(struct form *form)
{
	form->state = FORM_READ;
}


static void exit_read_mode(struct form *form, struct form_entry *entry)
{
	form->state = FORM_IDLE;

	if (entry->buf_len + 1 < FORM_ENTRY_TEXT_LEN) {
		attroff(A_BOLD);
		form_entry_append(form, entry, '_');
	}
}

static void form_read_iteration(struct form *form)
{
	struct form_entry *entry = form->entries + form->curr_entry;

	if (entry->buf_len + 1 <= FORM_ENTRY_TEXT_LEN) {
		attron(A_BOLD);
		form_entry_append(form, entry, 'I');
		attroff(A_BOLD);
	}

	const int c = getch();

	switch (c) {
	case ESCAPE_KEY:
		exit_read_mode(form, entry);
		break;

	case BACKSPACE_KEY: {
		if (entry->buf_len > 0) {
			form_entry_append(form, entry, '_');
			entry->buf_len--;
		}
	} break;

	default: {
		if (isalpha(c) && entry->buf_len + 1 <= FORM_ENTRY_TEXT_LEN) {
			form_entry_append(form, entry, c);
			entry->buf[entry->buf_len++] = c;
		}
	}
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

