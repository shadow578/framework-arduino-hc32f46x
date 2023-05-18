#include "init.h"
#include "../drivers/sysclock/sysclock.h"
#include "../drivers/sysclock/systick.h"
#include "../drivers/adc/adc.h"
//#include "../interrupts.h"
#include <hc32_ddl.h>

/**
 * set flash latency and cache 
 */
inline void flash_init()
{
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_5);
    EFM_InstructionCacheCmd(Enable);
    EFM_Lock();
}

void core_init()
{
#if defined(__CC_ARM) && defined(__TARGET_FPU_VFP)
    SCB->CPACR |= 0x00F00000;
#endif

    // setup VTO register
    SCB->VTOR = (uint32_t(LD_FLASH_START) & SCB_VTOR_TBLOFF_Msk);

    // setup the SoC and initialize drivers
    flash_init();
    sysclock_init();
    systick_init();
    //TODO: commented for now: interrupts_init();
    adc_init();
}
