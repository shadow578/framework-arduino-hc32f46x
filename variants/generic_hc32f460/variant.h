/*
 Copyright (c) 2015 Arduino LLC.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef BOARD_VARIANT_H_
#define BOARD_VARIANT_H_

//
// GPIO pin count (size of PIN_MAP array)
//
#define BOARD_NR_GPIO_PINS 83

//
// GPIO pin aliases (index into PIN_MAP array)
//

// PAx
#define PA0 0
#define PA1 1
#define PA2 2
#define PA3 3
#define PA4 4
#define PA5 5
#define PA6 6
#define PA7 7
#define PA8 8
#define PA9 9
#define PA10 10
#define PA11 11
#define PA12 12
#define PA13 13
#define PA14 14
#define PA15 15

// PBx
#define PB0 16
#define PB1 17
#define PB2 18
#define PB3 19
#define PB4 20
#define PB5 21
#define PB6 22
#define PB7 23
#define PB8 24
#define PB9 25
#define PB10 26
#define PB11 27
#define PB12 28
#define PB13 29
#define PB14 30
#define PB15 31

// PCx
#define PC0 32
#define PC1 33
#define PC2 34
#define PC3 35
#define PC4 36
#define PC5 37
#define PC6 38
#define PC7 39
#define PC8 40
#define PC9 41
#define PC10 42
#define PC11 43
#define PC12 44
#define PC13 45
#define PC14 46
#define PC15 47

// PDx
#define PD0 48
#define PD1 49
#define PD2 50
#define PD3 51
#define PD4 52
#define PD5 53
#define PD6 54
#define PD7 55
#define PD8 56
#define PD9 57
#define PD10 58
#define PD11 59
#define PD12 60
#define PD13 61
#define PD14 62
#define PD15 63

// PEx
#define PE0 64
#define PE1 65
#define PE2 66
#define PE3 67
#define PE4 68
#define PE5 69
#define PE6 70
#define PE7 71
#define PE8 72
#define PE9 73
#define PE10 74
#define PE11 75
#define PE12 76
#define PE13 77
#define PE14 78
#define PE15 79

// PHx
#define PH0 80
#define PH1 81
#define PH2 82

//
// USART gpio pins
//
#define BOARD_USART1_TX_PIN PC0
#define BOARD_USART1_RX_PIN PC1

#define BOARD_USART2_TX_PIN PA9
#define BOARD_USART2_RX_PIN PA15

#define BOARD_USART3_TX_PIN PE5
#define BOARD_USART3_RX_PIN PE4

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

#endif /* BOARD_VARIANT_H_ */
