#pragma once
#include <stdint.h>

/**
 * @brief calculate the baudrate realized with the given configuration
 * @param usartBaseClock the base clock of the USART peripheral (PCLK1 / pre-divider set by USART_PR.PSC)
 * @param DIV_integer the integer part of the baudrate divider (USART_BRR.DIV_Integer)
 * @param DIV_fraction the fractional part of the baudrate divider (USART_BRR.DIV_Fraction)
 * @param over8 the oversampling mode (0: 16-bit, 1: 8-bit; USART_CR1.OVER8)
 * @param useFractionalDivider true if the fractional divider is used, false if not (USART_CR1.FBME)
 * @return the baudrate realized with the given configuration
 */
static inline float calculateBaudrate(uint32_t usartBaseClock, uint32_t DIV_integer, uint32_t DIV_fraction, uint8_t over8, bool useFractionalDivider)
{
    // calculate the baudrate realized with the calculated dividers
    if (useFractionalDivider) {
        // calculation with fractional divider, see ref. manual page 621, Table 25-10
        return ((float)usartBaseClock * (128.0f + (float)DIV_fraction)) / (8.0f * (2.0f - (float)over8) * ((float)DIV_integer + 1.0f) * 256.0f);
    } else {
        // calculate without fractional divider, see ref. manual page 621, Table 25-9
        return (float)usartBaseClock / (8.0f * (2.0f - (float)over8) * ((float)DIV_integer + 1.0f));
    }
}
