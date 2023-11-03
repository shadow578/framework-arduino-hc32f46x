#ifndef __CORE_HOOKS_H
#define __CORE_HOOKS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DEF_HOOK(name, ...) __attribute__((weak)) extern void core_hook_##name(__VA_ARGS__);

    /**
     * called before setup(), but after SoC and core drivers are initialized
     */
    DEF_HOOK(pre_setup)

    /**
     * called after setup(), but before loop()
     */
    DEF_HOOK(post_setup)

    /**
     * called before every call to loop()
     */
    DEF_HOOK(loop)

    /**
     * shared USART transmit IRQ hook
     *
     * @param data the data byte about to be transmitted
     * @param usart_channel the usart channel. (one of [1,2,3,4]; 1 => M4_USART1)
     * @note runs inside a IRQ, so keep it short and sweet
     */
    DEF_HOOK(usart_tx_irq, uint8_t data, uint8_t usart_channel);

    /**
     * shared USART receive IRQ hook
     *
     * @param data the data byte that was received
     * @param usart_channel the usart channel. (one of [1,2,3,4]; 1 => M4_USART1)
     * @note runs inside a IRQ, so keep it short and sweet
     */
    DEF_HOOK(usart_rx_irq, uint8_t data, uint8_t usart_channel);

    /**
     * hook for watchdog reload during yield()
     *
     * @note IWatchdog library uses this hook to reload the watchdog timer
     */
    DEF_HOOK(yield_wdt_reload);

    /**
     * hook for system clock initialization
     *
     * @note
     * if this hook is not implemented, the cpu defaults to MCR at 8 MHz.
     * the default is restored by the core unless 'CORE_DONT_RESTORE_DEFAULT_CLOCKS' is defined.
     *
     * @note
     * use "drivers/sysclock/sysclock_util.h" to help with clock configuration
     *
     * @note
     * example workflow:
     * 1. configure flash / sram wait cycles (if needed)
     * 2. enable oscillator (e.g. XTAL)
     * 3. enable and configure PLL
     * 4. configure system clock dividers
     * 5. call power_mode_update_pre()
     * 6. switch system clock source
     * 7. call power_mode_update_post()
     */
    DEF_HOOK(sysclock_init);
#ifdef __cplusplus
}
#endif
#endif // __CORE_HOOKS_H
