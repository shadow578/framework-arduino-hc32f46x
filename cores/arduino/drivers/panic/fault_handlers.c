/**
 * hard fault handler based on:
 * - https://blog.feabhas.com/2018/09/updated-developing-a-generic-hard-fault-handler-for-arm-cortex-m3-cortex-m4-using-gcc/
 * - https://www.keil.com/appnotes/files/apnt209.pdf
 */
#include "fault_handlers.h"
#include "panic.h"
#include <hc32_ddl.h>

void fault_handlers_init()
{
    // enable cpu traps:
    // - divide by zero
    // - unaligned access
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk; //| SCB_CCR_UNALIGN_TRP_Msk;
}

/**
 * @brief append usage fault info to panic message
 */
void append_extended_fault_info()
{
#define CHECK_AND_APPEND(flag, fmt)        \
    if ((SCB->CFSR & (flag)) != 0)         \
    {                                      \
        panic_printf("- " fmt "\n"); \
    }

    // check and append memory management fault flags
    if ((SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) != 0)
    {
        panic_printf("Memory Management Fault:\n");
        CHECK_AND_APPEND(SCB_CFSR_MMARVALID_Msk, "MMARVALID");
        CHECK_AND_APPEND(SCB_CFSR_MLSPERR_Msk, "MLSPERR");
        CHECK_AND_APPEND(SCB_CFSR_MSTKERR_Msk, "MSTKERR");
        CHECK_AND_APPEND(SCB_CFSR_MUNSTKERR_Msk, "MUNSTKERR");
        CHECK_AND_APPEND(SCB_CFSR_DACCVIOL_Msk, "DACCVIOL");
        CHECK_AND_APPEND(SCB_CFSR_IACCVIOL_Msk, "IACCVIOL");
        panic_printf("\n");
    }

    // check and append bus fault flags
    if ((SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) != 0)
    {
        panic_printf("Bus Fault:\n");
        CHECK_AND_APPEND(SCB_CFSR_BFARVALID_Msk, "BFARVALID");
        CHECK_AND_APPEND(SCB_CFSR_LSPERR_Msk, "LSPERR");
        CHECK_AND_APPEND(SCB_CFSR_STKERR_Msk, "STKERR");
        CHECK_AND_APPEND(SCB_CFSR_UNSTKERR_Msk, "UNSTKERR");
        CHECK_AND_APPEND(SCB_CFSR_IMPRECISERR_Msk, "IMPRECISERR");
        CHECK_AND_APPEND(SCB_CFSR_PRECISERR_Msk, "PRECISERR");
        CHECK_AND_APPEND(SCB_CFSR_IBUSERR_Msk, "IBUSERR");
        panic_printf("\n");
    }

    // check and append usage fault flags
    if ((SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) != 0)
    {
        panic_printf("Usage fault:\n");
        CHECK_AND_APPEND(SCB_CFSR_DIVBYZERO_Msk, "DIVBYZERO");
        CHECK_AND_APPEND(SCB_CFSR_UNALIGNED_Msk, "UNALIGNED");
        CHECK_AND_APPEND(SCB_CFSR_NOCP_Msk, "NOCP");
        CHECK_AND_APPEND(SCB_CFSR_INVPC_Msk, "INVPC");
        CHECK_AND_APPEND(SCB_CFSR_INVSTATE_Msk, "INVSTATE");
        CHECK_AND_APPEND(SCB_CFSR_UNDEFINSTR_Msk, "UNDEFINSTR");
        panic_printf("\n");
    }
}

/**
 * @brief append stack dump to panic message
 */
void append_stack_dump(uint32_t stack[])
{
    panic_printf("R0 = 0x%08lx\n", stack[0]);
    panic_printf("R1 = 0x%08lx\n", stack[1]);
    panic_printf("R2 = 0x%08lx\n", stack[2]);
    panic_printf("R3 = 0x%08lx\n", stack[3]);
    panic_printf("R12 = 0x%08lx\n", stack[4]);
    panic_printf("LR = 0x%08lx\n", stack[5]);
    panic_printf("PC = 0x%08lx\n", stack[6]);
    panic_printf("PSR = 0x%08lx\n", stack[7]);
}

/**
 * @brief hard fault handler in C, called by assembly wrapper
 */
void HardFault_Handler_C(uint32_t stack[])
{
    // prepare panic message formatting
    panic_begin();

    // create panic message:
    // - nice-ish header
    panic_printf("\n\n*** HARD FAULT ***\n");

    // - HFSR
    panic_printf("SCB->HFSR = 0x%08lx\n", SCB->HFSR);

    // - forced hard fault info
    if ((SCB->HFSR & SCB_HFSR_FORCED_Msk) != 0)
    {
        panic_printf("- Forced Hard Fault -\n");
        panic_printf("SCB->CFSR = 0x%08lx\n", SCB->CFSR);

        // - extended fault info:
        //  * memory management fault
        //  * bus fault
        //  * usage fault
        append_extended_fault_info();
    }

    // - stack dump
    panic_printf("- Stack -\n");
    append_stack_dump(stack);

    // - nice-ish footer
    panic_printf("\n*** END HARD FAULT ***\n\n");

    // end panic message and halt
    panic_end();
}

/**
 * @brief hard fault handler wrapper in assembly
 */
__attribute__((naked)) void HardFault_Handler(void)
{
    asm volatile(
        " mrs r0,msp    \n"
        " b HardFault_Handler_C \n");
}
