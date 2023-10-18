#ifndef CORE_DISABLE_FAULT_HANDLER
/**
 * hard fault handler based on:
 * - https://blog.feabhas.com/2018/09/updated-developing-a-generic-hard-fault-handler-for-arm-cortex-m3-cortex-m4-using-gcc/
 * - https://www.keil.com/appnotes/files/apnt209.pdf
 */
#include "fault_handlers.h"
#include "panic.h"
#include <hc32_ddl.h>

typedef union hardfault_stack_frame_t
{
    struct
    {
        uint32_t r0;
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r12;
        uint32_t lr;
        uint32_t pc;
        uint32_t psr;
    };
    uint32_t raw[8];
} hardfault_stack_frame_t;

void fault_handlers_init()
{
    // enable cpu traps:
    // - divide by zero
    // - unaligned access
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk; //| SCB_CCR_UNALIGN_TRP_Msk;
}

/**
 * @brief print extended fault info to panic output. Prints flags from CFSR register.
 */
void print_cfsr_info()
{
#define CHECK_AND_APPEND(flag, fmt)   \
    if ((SCB->CFSR & (flag)) != 0)    \
    {                                 \
        panic_printf(" * " fmt "\n"); \
    }

    // check and append memory management fault flags
    if ((SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) != 0)
    {
        panic_printf("- Memory Management Fault:\n");
        CHECK_AND_APPEND(SCB_CFSR_MMARVALID_Msk, "MMARVALID");
        CHECK_AND_APPEND(SCB_CFSR_MLSPERR_Msk, "MLSPERR");
        CHECK_AND_APPEND(SCB_CFSR_MSTKERR_Msk, "MSTKERR");
        CHECK_AND_APPEND(SCB_CFSR_MUNSTKERR_Msk, "MUNSTKERR");
        CHECK_AND_APPEND(SCB_CFSR_DACCVIOL_Msk, "DACCVIOL");
        CHECK_AND_APPEND(SCB_CFSR_IACCVIOL_Msk, "IACCVIOL");
    }

    // check and append bus fault flags
    if ((SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) != 0)
    {
        panic_printf("- Bus Fault:\n");
        CHECK_AND_APPEND(SCB_CFSR_BFARVALID_Msk, "BFARVALID");
        CHECK_AND_APPEND(SCB_CFSR_LSPERR_Msk, "LSPERR");
        CHECK_AND_APPEND(SCB_CFSR_STKERR_Msk, "STKERR");
        CHECK_AND_APPEND(SCB_CFSR_UNSTKERR_Msk, "UNSTKERR");
        CHECK_AND_APPEND(SCB_CFSR_IMPRECISERR_Msk, "IMPRECISERR");
        CHECK_AND_APPEND(SCB_CFSR_PRECISERR_Msk, "PRECISERR");
        CHECK_AND_APPEND(SCB_CFSR_IBUSERR_Msk, "IBUSERR");
    }

    // check and append usage fault flags
    if ((SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) != 0)
    {
        panic_printf("- Usage fault:\n");
        CHECK_AND_APPEND(SCB_CFSR_DIVBYZERO_Msk, "DIVBYZERO");
        CHECK_AND_APPEND(SCB_CFSR_UNALIGNED_Msk, "UNALIGNED");
        CHECK_AND_APPEND(SCB_CFSR_NOCP_Msk, "NOCP");
        CHECK_AND_APPEND(SCB_CFSR_INVPC_Msk, "INVPC");
        CHECK_AND_APPEND(SCB_CFSR_INVSTATE_Msk, "INVSTATE");
        CHECK_AND_APPEND(SCB_CFSR_UNDEFINSTR_Msk, "UNDEFINSTR");
    }
}

/**
 * @brief print hardfault stack frame to panic output
 */
void print_stack_frame(hardfault_stack_frame_t *stack_frame)
{
    panic_printf("R0 = 0x%08lx\n", stack_frame->r0);
    panic_printf("R1 = 0x%08lx\n", stack_frame->r1);
    panic_printf("R2 = 0x%08lx\n", stack_frame->r2);
    panic_printf("R3 = 0x%08lx\n", stack_frame->r3);
    panic_printf("R12 = 0x%08lx\n", stack_frame->r12);
    panic_printf("LR = 0x%08lx\n", stack_frame->lr);
    panic_printf("PC = 0x%08lx\n", stack_frame->pc);
    panic_printf("PSR = 0x%08lx\n", stack_frame->psr);
}

/**
 * @brief hard fault handler in C, called by assembly wrapper
 */
void HardFault_Handler_C(hardfault_stack_frame_t *stack_frame, uint32_t lr_value)
{
    // prepare panic message formatting
    panic_begin();

    // print panic message:
    // - header
    panic_printf("\n\n*** HARDFAULT ***\n");

    // - fault status registers
    panic_printf("- FSR / FAR:\n");
    panic_printf("SCB->HFSR = 0x%08lx\n", SCB->HFSR);
    panic_printf("SCB->CFSR = 0x%08lx\n", SCB->CFSR);
    panic_printf("SCB->DFSR = 0x%08lx\n", SCB->DFSR);
    panic_printf("SCB->AFSR = 0x%08lx\n", SCB->AFSR);

    if ((SCB->CFSR & SCB_CFSR_MMARVALID_Msk) != 0)
    {
        panic_printf("SCB->MMFAR = 0x%08lx\n", SCB->MMFAR);
    }

    if ((SCB->CFSR & SCB_CFSR_BFARVALID_Msk) != 0)
    {
        panic_printf("SCB->BFAR = 0x%08lx\n", SCB->BFAR);
    }

    // - CFSR flag names
    //  * memory management fault
    //  * bus fault
    //  * usage fault
#ifndef HARDFAULT_EXCLUDE_CFSR_INFO
    if ((SCB->HFSR & SCB_HFSR_FORCED_Msk) != 0)
    {
        print_cfsr_info();
    }
#endif

    // - stack frame
    panic_printf("- Stack frame:\n");
    print_stack_frame(stack_frame);

    // - misc
    //  * LR value
    panic_printf("- Misc:\n");
    panic_printf("LR = 0x%08lx\n", lr_value);

    // - footer
    panic_printf("***\n\n");

    // end panic message and halt
    panic_end();
}

/**
 * @brief hard fault handler wrapper in assembly
 */
__attribute__((naked)) void HardFault_Handler(void)
{
    asm volatile(
        " tst lr, #4            \n" // check if we're using the MSP or PSP
        " ite eq                \n" // if equal, we're using the MSP
        " mrseq r0, msp         \n" //  * using MSP, load MSP into r0
        " mrsne r0, psp         \n" //  * using PSP, load PSP into r0
        " mov r1, lr            \n" // load LR into r1
        " b HardFault_Handler_C \n" // call handler in C
    );
}

#else
__attribute__((weak)) void fault_handlers_init() {}
#endif // CORE_DISABLE_FAULT_HANDLER
