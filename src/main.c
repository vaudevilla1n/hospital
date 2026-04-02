#include "tui.h"

int main(void)
{
	tui_init();

	while (!tui_exited)
		tui_iteration();

	tui_deinit();
}
