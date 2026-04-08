#include "file.h"

#include "util.h"

#include <stdio.h>

static void write_entry(FILE *f, const char *field, const char *value)
{
	fprintf(f, "\t%s: '%s',\n", field, value);
}

static inline void write_str(FILE *f, const char *s, const ptrdiff_t len)
{
	if (!len)
		return;

	fwrite(s, sizeof(*s), len, f);
}

static void write_entry_sized(FILE *f, const char *field, const ptrdiff_t fieldlen,
		const char *value, const ptrdiff_t valuelen)
{
	fputc('\t', f);
	write_str(f, field, fieldlen);
	fputs(": '", f);
	write_str(f, value, valuelen);
	fputs("',\n", f);
}

void file_save_forms(const struct menu *menu, const enum tui_state menu_type)
{
	FILE *f = fopen("save.txt", "a+");

	if (!f)
		return;

	const char *type = nullptr;

	switch (menu_type) {
	case TUI_ADD_PATIENT:		type = "patient"; break;
	case TUI_ADD_APPOINTMENT:	type = "appointment"; break;

	default:			break;
	}

	if (!type)
		return;

	fprintf(f, "{\n");
	write_entry(f, "type", type);
	for (ptrdiff_t i = 0; i < menu->nopts; i++) {
		const struct option *opt = menu->opts + i;

		if (!is_form(opt))
			continue;

		const struct form *form = &opt->as.form;

		write_entry_sized(f, opt->text, opt->textlen, form->buf, form->buflen);
	}
	fprintf(f, "}\n");

	fclose(f);
}
