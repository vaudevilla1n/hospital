#pragma once

#include <stdbool.h>

extern bool tui_exited;

void tui_init(void);
void tui_deinit(void);

void tui_iteration(void);
