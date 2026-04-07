#pragma once

#include <curses.h>
#include <stdbool.h>

#define ENTER_KEY	'\n'
#define ESCAPE_KEY	'\x1b'
#define BACKSPACE_KEY	'\b'
#define DELETE_KEY	'\x7f'

enum tui_state {
	TUI_NONE,

	TUI_MAIN_MENU,

	TUI_ADD,
	TUI_ADD_PATIENT,
	TUI_ADD_APPOINTMENT,

	TUI_ADD_SAVE,

	TUI_VIEW,
	TUI_SEARCH,
	TUI_EDIT,
	TUI_EXIT,
};

#define TUI_TOTAL_STATES	TUI_EXIT

extern bool tui_exited;
extern WINDOW *tui_window;

void tui_init(void);
void tui_deinit(void);

void tui_iteration(void);
