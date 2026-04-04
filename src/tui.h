#pragma once

#include <curses.h>
#include <stdbool.h>

#define ENTER_KEY	'\n'
#define ESCAPE_KEY	'\x1b'
#define BACKSPACE_KEY	'\b'
#define DELETE_KEY	'\x7f'

#define TUI_TITLE_X	(COLS * 1/10)
#define TUI_TITLE_Y	(LINES * 1/10)

#define TUI_PAGE_X	(COLS * 3/8)
#define TUI_PAGE_Y	(LINES * 3/10)

#define TUI_SPACE_Y	4

enum tui_state {
	TUI_NONE,

	TUI_MAIN_MENU,

	TUI_ADD,
	TUI_ADD_PATIENT,

	TUI_VIEW,
	TUI_SEARCH,
	TUI_EDIT,
	TUI_EXIT,
};

extern bool tui_exited;

void tui_init(void);
void tui_deinit(void);

void tui_draw_border(void);
void tui_draw_title(const char *title);

void tui_iteration(void);
