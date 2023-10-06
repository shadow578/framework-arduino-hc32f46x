#include "../Arduino.h"
#include "init.h"
#include "../core_debug.h"
#include "../core_hooks.h"

int main(void)
{
	// initialize SoC, then CORE_DEBUG
	core_init();
	CORE_DEBUG_INIT();

	// call setup()
	core_hook_pre_setup();
	CORE_DEBUG_PRINTF("core entering setup\n");
	setup();
	core_hook_post_setup();
	
	// call loop() forever
	CORE_DEBUG_PRINTF("core entering main loop\n");
	while (1)
	{
		core_hook_loop();
		loop();
	}

	CORE_ASSERT_FAIL("main loop exited");
}
