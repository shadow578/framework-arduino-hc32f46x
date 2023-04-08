//
// builder configured ddl_config
// ddl options are enabled by adding a define to the compiler options (eg. -D DDL_ADC_ENABLE)
//

#ifndef __DDL_CONFIG_H__
#define __DDL_CONFIG_H__

//
// Driver Libraries (configured by board manifest)
//
#define DDL_ON 1u
#define DDL_OFF 0u

// ICG and UTILITY are always on
// they are required by some of the other drivers
// and it would be a pain to check if they are needed
#define DDL_ICG_ENABLE DDL_ON
#define DDL_UTILITY_ENABLE DDL_ON

// other drivers are configured by the board manifest
// if the macro for the ddl is defined, it is redefine to DDL_ON
// otherwise it is defined as DDL_OFF
#if defined(DDL_ADC_ENABLE)
#undef DDL_ADC_ENABLE
#define DDL_ADC_ENABLE DDL_ON
#else
#define DDL_ADC_ENABLE DDL_OFF
#endif

#if defined(DDL_AES_ENABLE)
#undef DDL_AES_ENABLE
#define DDL_AES_ENABLE DDL_ON
#else
#define DDL_AES_ENABLE DDL_OFF
#endif

#if defined(DDL_CAN_ENABLE)
#undef DDL_CAN_ENABLE
#define DDL_CAN_ENABLE DDL_ON
#else
#define DDL_CAN_ENABLE DDL_OFF
#endif

#if defined(DDL_CMP_ENABLE)
#undef DDL_CMP_ENABLE
#define DDL_CMP_ENABLE DDL_ON
#else
#define DDL_CMP_ENABLE DDL_OFF
#endif

#if defined(DDL_CLK_ENABLE)
#undef DDL_CLK_ENABLE
#define DDL_CLK_ENABLE DDL_ON
#else
#define DDL_CLK_ENABLE DDL_OFF
#endif

#if defined(DDL_DCU_ENABLE)
#undef DDL_DCU_ENABLE
#define DDL_DCU_ENABLE DDL_ON
#else
#define DDL_DCU_ENABLE DDL_OFF
#endif

#if defined(DDL_DMAC_ENABLE)
#undef DDL_DMAC_ENABLE
#define DDL_DMAC_ENABLE DDL_ON
#else
#define DDL_DMAC_ENABLE DDL_OFF
#endif

#if defined(DDL_EFM_ENABLE)
#undef DDL_EFM_ENABLE
#define DDL_EFM_ENABLE DDL_ON
#else
#define DDL_EFM_ENABLE DDL_OFF
#endif

#if defined(DDL_EMB_ENABLE)
#undef DDL_EMB_ENABLE
#define DDL_EMB_ENABLE DDL_ON
#else
#define DDL_EMB_ENABLE DDL_OFF
#endif

#if defined(DDL_EXINT_NMI_SWI_ENABLE)
#undef DDL_EXINT_NMI_SWI_ENABLE
#define DDL_EXINT_NMI_SWI_ENABLE DDL_ON
#else
#define DDL_EXINT_NMI_SWI_ENABLE DDL_OFF
#endif

#if defined(DDL_GPIO_ENABLE)
#undef DDL_GPIO_ENABLE
#define DDL_GPIO_ENABLE DDL_ON
#else
#define DDL_GPIO_ENABLE DDL_OFF
#endif

#if defined(DDL_HASH_ENABLE)
#undef DDL_HASH_ENABLE
#define DDL_HASH_ENABLE DDL_ON
#else
#define DDL_HASH_ENABLE DDL_OFF
#endif

#if defined(DDL_I2C_ENABLE)
#undef DDL_I2C_ENABLE
#define DDL_I2C_ENABLE DDL_ON
#else
#define DDL_I2C_ENABLE DDL_OFF
#endif

#if defined(DDL_I2S_ENABLE)
#undef DDL_I2S_ENABLE
#define DDL_I2S_ENABLE DDL_ON
#else
#define DDL_I2S_ENABLE DDL_OFF
#endif

#if defined(DDL_INTERRUPTS_ENABLE)
#undef DDL_INTERRUPTS_ENABLE
#define DDL_INTERRUPTS_ENABLE DDL_ON
#else
#define DDL_INTERRUPTS_ENABLE DDL_OFF
#endif

#if defined(DDL_KEYSCAN_ENABLE)
#undef DDL_KEYSCAN_ENABLE
#define DDL_KEYSCAN_ENABLE DDL_ON
#else
#define DDL_KEYSCAN_ENABLE DDL_OFF
#endif

#if defined(DDL_MPU_ENABLE)
#undef DDL_MPU_ENABLE
#define DDL_MPU_ENABLE DDL_ON
#else
#define DDL_MPU_ENABLE DDL_OFF
#endif

#if defined(DDL_OTS_ENABLE)
#undef DDL_OTS_ENABLE
#define DDL_OTS_ENABLE DDL_ON
#else
#define DDL_OTS_ENABLE DDL_OFF
#endif

#if defined(DDL_PGA_ENABLE)
#undef DDL_PGA_ENABLE
#define DDL_PGA_ENABLE DDL_ON
#else
#define DDL_PGA_ENABLE DDL_OFF
#endif

#if defined(DDL_PWC_ENABLE)
#undef DDL_PWC_ENABLE
#define DDL_PWC_ENABLE DDL_ON
#else
#define DDL_PWC_ENABLE DDL_OFF
#endif

#if defined(DDL_QSPI_ENABLE)
#undef DDL_QSPI_ENABLE
#define DDL_QSPI_ENABLE DDL_ON
#else
#define DDL_QSPI_ENABLE DDL_OFF
#endif

#if defined(DDL_RMU_ENABLE)
#undef DDL_RMU_ENABLE
#define DDL_RMU_ENABLE DDL_ON
#else
#define DDL_RMU_ENABLE DDL_OFF
#endif

#if defined(DDL_RTC_ENABLE)
#undef DDL_RTC_ENABLE
#define DDL_RTC_ENABLE DDL_ON
#else
#define DDL_RTC_ENABLE DDL_OFF
#endif

#if defined(DDL_SDIOC_ENABLE)
#undef DDL_SDIOC_ENABLE
#define DDL_SDIOC_ENABLE DDL_ON
#else
#define DDL_SDIOC_ENABLE DDL_OFF
#endif

#if defined(DDL_SPI_ENABLE)
#undef DDL_SPI_ENABLE
#define DDL_SPI_ENABLE DDL_ON
#else
#define DDL_SPI_ENABLE DDL_OFF
#endif

#if defined(DDL_SRAM_ENABLE)
#undef DDL_SRAM_ENABLE
#define DDL_SRAM_ENABLE DDL_ON
#else
#define DDL_SRAM_ENABLE DDL_OFF
#endif

#if defined(DDL_SWDT_ENABLE)
#undef DDL_SWDT_ENABLE
#define DDL_SWDT_ENABLE DDL_ON
#else
#define DDL_SWDT_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER0_ENABLE)
#undef DDL_TIMER0_ENABLE
#define DDL_TIMER0_ENABLE DDL_ON
#else
#define DDL_TIMER0_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER4_CNT_ENABLE)
#undef DDL_TIMER4_CNT_ENABLE
#define DDL_TIMER4_CNT_ENABLE DDL_ON
#else
#define DDL_TIMER4_CNT_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER4_EMB_ENABLE)
#undef DDL_TIMER4_EMB_ENABLE
#define DDL_TIMER4_EMB_ENABLE DDL_ON
#else
#define DDL_TIMER4_EMB_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER4_OCO_ENABLE)
#undef DDL_TIMER4_OCO_ENABLE
#define DDL_TIMER4_OCO_ENABLE DDL_ON
#else
#define DDL_TIMER4_OCO_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER4_PWM_ENABLE)
#undef DDL_TIMER4_PWM_ENABLE
#define DDL_TIMER4_PWM_ENABLE DDL_ON
#else
#define DDL_TIMER4_PWM_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER4_SEVT_ENABLE)
#undef DDL_TIMER4_SEVT_ENABLE
#define DDL_TIMER4_SEVT_ENABLE DDL_ON
#else
#define DDL_TIMER4_SEVT_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMER6_ENABLE)
#undef DDL_TIMER6_ENABLE
#define DDL_TIMER6_ENABLE DDL_ON
#else
#define DDL_TIMER6_ENABLE DDL_OFF
#endif

#if defined(DDL_TIMERA_ENABLE)
#undef DDL_TIMERA_ENABLE
#define DDL_TIMERA_ENABLE DDL_ON
#else
#define DDL_TIMERA_ENABLE DDL_OFF
#endif

#if defined(DDL_TRNG_ENABLE)
#undef DDL_TRNG_ENABLE
#define DDL_TRNG_ENABLE DDL_ON
#else
#define DDL_TRNG_ENABLE DDL_OFF
#endif

#if defined(DDL_USART_ENABLE)
#undef DDL_USART_ENABLE
#define DDL_USART_ENABLE DDL_ON
#else
#define DDL_USART_ENABLE DDL_OFF
#endif

#if defined(DDL_USBFS_ENABLE)
#undef DDL_USBFS_ENABLE
#define DDL_USBFS_ENABLE DDL_ON
#else
#define DDL_USBFS_ENABLE DDL_OFF
#endif

#if defined(DDL_WDT_ENABLE)
#undef DDL_WDT_ENABLE
#define DDL_WDT_ENABLE DDL_ON
#else
#define DDL_WDT_ENABLE DDL_OFF
#endif

//
// Middleware (always off)
//
#define MW_ON 1u
#define MW_OFF 0u

#define MW_SD_CARD_ENABLE MW_OFF
#define MW_USB_ENABLE MW_OFF

#endif // __DDL_CONFIG_H__
