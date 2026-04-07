#include "file.h"

#include <stdio.h>

void file_save_forms(const struct menu *menu)
{
	FILE *f = fopen("save.txt", "a+");

	if (!f)
		return;

	for (ptrdiff_t i = 0; i < menu->nopts; i++) {
		const struct option *opt = menu->opts + i;

		if (!is_form(opt))
			continue;

		const struct form *form = &opt->as.form;

		fprintf(f, "%.*s: %.*s\n", (int)opt->textlen, opt->text, (int)form->buflen, form->buf);
	}

	fclose(f);
}
