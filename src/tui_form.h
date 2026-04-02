#pragma once

#include "tui.h"
#include <stddef.h>

enum form_entry_type {
	FORM_TEXT,
	FORM_NUMBER,
};

struct form_entry {
	enum form_entry_type type;
	const char *label;
};

struct form {
	int x;
	int y;

	int space;

	ptrdiff_t curr_entry;
	ptrdiff_t nentries;
	struct form_entry *entries;
};

extern struct form tui_add_patient_form;

void init_forms(void);
void draw_form(const struct form *form);
enum tui_state form_iteration(struct form *form);
