#pragma once

#include "tui.h"
#include <stddef.h>

#define FORM_ENTRY_INPUT_MAX	32

#define FORM_ENTRY_TEXT_LEN	32
#define FORM_ENTRY_DATE_LEN	8

enum form_entry_type {
	FORM_ENTRY_TEXT,
	FORM_ENTRY_NUMBER,
};

struct form_entry {
	enum form_entry_type type;

	ptrdiff_t label_len;
	const char *label;

	ptrdiff_t buf_len;
	char buf[FORM_ENTRY_INPUT_MAX];
};

enum form_state {
	FORM_IDLE,
	FORM_READ,
};

struct form {
	int x;
	int y;

	int space;

	enum form_state state;

	ptrdiff_t curr_entry;
	ptrdiff_t nentries;
	struct form_entry *entries;
};

extern struct form tui_add_patient_form;

void init_forms(void);

enum tui_state add_patient_form_iteration(const bool update);
