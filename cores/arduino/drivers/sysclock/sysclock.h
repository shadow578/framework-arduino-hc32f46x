#pragma once
#include <stdint.h>

/**
 * @brief Initialize the system clock
 *
 * @note weak implementation, can be overriden by user code to allow custom clock configuration
 */
__attribute__((weak)) void sysclock_init();

/**
 * @brief get the current CPU clock frequency
 * @return CPU clock frequency in Hz
 *
 * @note equivalent to hclk frequency (see User Manual, Section 4.3, Table 4-1)
 */
#define get_cpu_clock() get_hclk_clock()

//#region clock sources getters
/**
 * @brief get the current system clock frequency
 * @return clock frequency in Hz
 * @note clock source to the other clock sources
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_system_clock();

/**
 * @brief get the current HCLK clock frequency
 * @return clock frequency in Hz
 * @note clock source of the CPU, DMA*, EMF, SRAM*, MPU, GPIO, DCU, INTC, and QSPI peripherals
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_hclk_clock();

/**
 * @brief get the current PCLK0 clock frequency
 * @return clock frequency in Hz
 * @note clock source of Timer6 Counters
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_pclk0_clock();

/**
 * @brief get the current PCLK1 clock frequency
 * @return clock frequency in Hz
 * @note clock source of the USART*, SPI*, USBFS, Timer0, TimerA, Timer4, Timer6 EMB, CRC, HASH, AES, I2S* peripherals
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_pckl1_clock();

/**
 * @brief get the current PCLK2 clock frequency
 * @return clock frequency in Hz
 * @note clock source for ADC Transformation
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_pclk2_clock();

/**
 * @brief get the current PCLK3 clock frequency
 * @return clock frequency in Hz
 * @note clock source of the RTC (control), I2C*, CMP, WDT, SWDT (control) peripherals
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_pclk3_clock();

/**
 * @brief get the current pclk4 clock frequency
 * @return clock frequency in Hz
 * @note clock source of the ADC (control logic), TRNG peripherals
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_pclk4_clock();

/**
 * @brief get the current EXCLK clock frequency
 * @return clock frequency in Hz
 * @note clock source of the SDIO*, CAN peripherals
 * @note see User Manual, Section 4.3, Table 4-1
 */
uint32_t get_exclk_clock();

//#endregion
