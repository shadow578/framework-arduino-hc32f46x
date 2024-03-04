#pragma once
#ifndef __HC32_DDL_H__
#define __HC32_DDL_H__

typedef struct {} M4_ADC_TypeDef;
typedef enum {} en_adc_resolution_t;
typedef enum {} en_adc_data_align_t;
typedef enum {} en_adc_scan_mode_t;

typedef uint16_t en_port_t;
typedef uint16_t en_pin_t;
typedef enum {} en_port_func_t;

typedef uint16_t IRQn_Type;
typedef enum {} en_int_src_t;
typedef void (*func_ptr_t)(void);

typedef struct {} M4_USART_TypeDef;
typedef struct {} stc_usart_uart_init_t;

#endif // __HC32_DDL_H__
