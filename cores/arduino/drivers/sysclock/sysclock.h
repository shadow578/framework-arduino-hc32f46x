#pragma once
#include <stdint.h>

/**
 * @brief system clock frequencies
 * @note all frequencies are in Hz
 * @note see User Manual, Section 4.3, Table 4-1 for more information on the different clocks
 */
typedef struct system_clock_frequencies
{
    /**
     * @brief system clock frequency
     * @note master clock source for all other clock sources
     */
    uint32_t system;

    /**
     * @brief HCLK clock frequency
     * @note clock source of the CPU, DMA*, EMF, SRAM*, MPU, GPIO, DCU, INTC, and QSPI peripherals
     */
    uint32_t hclk;

    /**
     * @brief PCLK0 clock frequency
     * @note clock source of Timer6 Counters
     */
    uint32_t pclk0;

    /**
     * @brief PCLK1 clock frequency
     * @note clock source of the USART*, SPI*, USBFS, Timer0, TimerA, Timer4, Timer6 EMB, CRC, HASH, AES, I2S* peripherals
     */
    uint32_t pclk1;

    /**
     * @brief PCLK2 clock frequency
     * @note clock source for ADC Transformation
     */
    uint32_t pclk2;

    /**
     * @brief PCLK3 clock frequency
     * @note clock source of the RTC (control), I2C*, CMP, WDT, SWDT (control) peripherals
     */
    uint32_t pclk3;

    /**
     * @brief PCLK4 clock frequency
     * @note clock source of the ADC (control logic), TRNG peripherals
     */
    uint32_t pclk4;

    /**
     * @brief EXCLK clock frequency
     * @note clock source of the SDIO*, CAN peripherals
     */
    uint32_t exclk;
} system_clock_frequencies_t;

/**
 * @brief current system clock frequencies
 * @note this variable is updated by update_system_clock_frequencies()
 */
extern system_clock_frequencies_t SYSTEM_CLOCK_FREQUENCIES;

/**
 * @brief update the SYSTEM_CLOCK_FREQUENCIES variable
 */
void update_system_clock_frequencies();
