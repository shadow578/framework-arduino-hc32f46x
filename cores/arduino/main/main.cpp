#include "../Arduino.h"
#include "init.h"
#include "../core_debug.h"
#include "../core_hooks.h"

int main(void)
{
	board_init();
	CORE_DEBUG_INIT();
	core_hook_pre_setup();
	CORE_DEBUG_PRINTF("core entering setup\n");
	setup();
	CORE_DEBUG_PRINTF("core entering main loop\n");
	core_hook_post_setup();
	while (1)
	{
		core_hook_loop();
		loop();
	}
}
