#pragma once
#include <hc32_ddl.h>
#include <addon_gpio.h>
#include "core_debug.h"
#include "../adc/adc.h"
#include "../../WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif

//
// GPIO wrappers for PORT_* functions
//
#define ASSERT_GPIO_PIN(gpio_pin) \
    CORE_ASSERT(IS_GPIO_PIN(gpio_pin), "GPIO_* called with invalid gpio pin")
#define PIN_ARG(gpio_pin) PIN_MAP[gpio_pin].port, PIN_MAP[gpio_pin].bit_mask

    /**
     * @brief GPIO wrapper for PORT_Init
     */
    inline en_result_t GPIO_Init(uint16_t gpio_pin, const stc_port_init_t *pstcPortInit)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_Init(PIN_ARG(gpio_pin), pstcPortInit);
    }

    /**
     * @brief GPIO wrapper for PORT_GetConfig
     */
    inline en_result_t GPIO_GetConfig(uint16_t gpio_pin, stc_port_init_t *pstcPortInit)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_GetConfig(PIN_ARG(gpio_pin), pstcPortInit);
    }

    /**
     * @brief GPIO wrapper for PORT_GetBit
     */
    inline en_flag_status_t GPIO_GetBit(uint16_t gpio_pin)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_GetBit(PIN_ARG(gpio_pin));
    }

    /**
     * @brief GPIO wrapper for PORT_OE
     */
    inline en_result_t GPIO_OE(uint16_t gpio_pin, en_functional_state_t enNewState)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_OE(PIN_ARG(gpio_pin), enNewState);
    }

    /**
     * @brief GPIO wrapper for PORT_SetBits
     */
    inline en_result_t GPIO_SetBits(uint16_t gpio_pin)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_SetBits(PIN_ARG(gpio_pin));
    }

    /**
     * @brief GPIO wrapper for PORT_ResetBits
     */
    inline en_result_t GPIO_ResetBits(uint16_t gpio_pin)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_ResetBits(PIN_ARG(gpio_pin));
    }

    /**
     * @brief GPIO wrapper for PORT_Toggle
     */
    inline en_result_t GPIO_Toggle(uint16_t gpio_pin)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_Toggle(PIN_ARG(gpio_pin));
    }

    /**
     * @brief GPIO wrapper for PORT_SetFunc
     * @param enFuncSelect GPIO pin primary function select
     * @param enSubFunc GPIO pin sub-function enable/disable (subfunction is GPIO output for most pins)
     */
    inline en_result_t GPIO_SetFunc(uint16_t gpio_pin, en_port_func_t enFuncSelect, en_functional_state_t enSubFunc = Disable)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_SetFunc(PIN_ARG(gpio_pin), enFuncSelect, enSubFunc);
    }

#ifdef __cplusplus
}
#endif
