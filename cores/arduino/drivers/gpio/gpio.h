#pragma once
#include <hc32_ddl.h>
#include <addon_gpio.h>
#include "../adc/adc.h"
#include "gpio_pindefs.h"

#ifdef __cplusplus
extern "C"
{
#endif

//
// number of pins
//
#define BOARD_NR_GPIO_PINS 83
#define BOARD_NR_ADC_PINS 16

//
// SPI gpio pins
//
#define BOARD_NR_SPI 3
#define BOARD_SPI1_NSS_PIN PA4
#define BOARD_SPI1_SCK_PIN PA5
#define BOARD_SPI1_MISO_PIN PA6
#define BOARD_SPI1_MOSI_PIN PA7

#define BOARD_SPI2_NSS_PIN PB12
#define BOARD_SPI2_SCK_PIN PB13
#define BOARD_SPI2_MISO_PIN PB14
#define BOARD_SPI2_MOSI_PIN PB15

#define BOARD_SPI3_NSS_PIN PA15
#define BOARD_SPI3_SCK_PIN PB3
#define BOARD_SPI3_MISO_PIN PB4
#define BOARD_SPI3_MOSI_PIN PB5

    //
    // GPIO pin map configuration
    //
    typedef struct pin_info_t
    {
        /**
         * @brief bit position of the pin in the port
         */
        uint8_t gpio_bit;

        /**
         * @brief IO port this pin belongs to
         */
        __IO en_port_t gpio_port;

        /**
         * @brief bit mask of the pin in the port
         */
        __IO en_pin_t gpio_pin;

        /**
         * @brief pointer to the ADC device of this pin, if any
         * @note NULL if not a ADC pin
         */
        adc_dev *adc_device;

        /**
         * @brief adc channel number of this pin, if any
         * @note ADC_PIN_INVALID if not a ADC pin
         */
        __IO uint8_t adc_channel;

        /**
         * @brief function of this GPIO pin, set by GPIO_SetFunc
         */
        __IO en_port_func_t function;
    } pin_info_t;

    /**
     * @brief GPIO pin map
     */
    extern const pin_info_t PIN_MAP[BOARD_NR_GPIO_PINS];

//
// @brief GPIO wrappers for PORT_* functions
//
#define IS_GPIO_PIN(gpio_pin) (gpio_pin >= 0 && gpio_pin < BOARD_NR_GPIO_PINS)
#define ASSERT_GPIO_PIN(gpio_pin) \
    if (!IS_GPIO_PIN(gpio_pin))   \
    {                             \
        return Error;             \
    }
#define PIN_ARG(gpio_pin) PIN_MAP[gpio_pin].gpio_port, PIN_MAP[gpio_pin].gpio_pin

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
        if (!IS_GPIO_PIN(gpio_pin))
        {
            return Reset;
        }

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
     * @note function select is chosen in PIN_MAP
     */
    inline en_result_t GPIO_SetFunc(uint16_t gpio_pin, en_functional_state_t state)
    {
        ASSERT_GPIO_PIN(gpio_pin);
        return PORT_SetFunc(PIN_ARG(gpio_pin), PIN_MAP[gpio_pin].function, state);
    }

#ifdef __cplusplus
}
#endif
