#include "init.h"
#include "../drivers/sysclock/sysclock.h"
#include "../drivers/sysclock/sysclock_util.h"
#include "../drivers/sysclock/systick.h"
#include "../drivers/panic/fault_handlers.h"
#include "../core_debug.h"
#include "../core_hooks.h"
#include <hc32_ddl.h>

/**
 * @brief check if the last reset was caused by a
 *        configuration fault (e.g. XTAL fault) that could be reoccuring
 */
inline void check_reoccuring_reset_fault()
{
    // get reset cause
    stc_rmu_rstcause_t cause;
    RMU_GetResetCause(&cause);

    // check for possibly reoccuring faults:
#define CHECK_RSTCAUSE(cause, msg) \
    if (cause == Set)              \
    {                              \
        RMU_ClrResetFlag();        \
        panic(msg);                \
    }

    // - XTAL error, could be caused by a invalid XTAL config or a bad circuit
    CHECK_RSTCAUSE(cause.enXtalErr, "XTAL error, check XTAL config and circuit");
}

void core_init()
{
#if defined(__CC_ARM) && defined(__TARGET_FPU_VFP)
    SCB->CPACR |= 0x00F00000;
#endif

    // setup vector table offset
    SCB->VTOR = (uint32_t(LD_FLASH_START) & SCB_VTOR_TBLOFF_Msk);

    // check if last reset could be reoccuring
    check_reoccuring_reset_fault();

    // setup fault handling
    fault_handlers_init();

    // initialize system clock:
    // - restore default clock settings
#ifndef CORE_DONT_RESTORE_DEFAULT_CLOCKS
    sysclock_restore_default_clocks();
#endif

    // - call user setup hook
    core_hook_sysclock_init();
    update_system_clock_frequencies();

    // initialize systick
    systick_init();
}
