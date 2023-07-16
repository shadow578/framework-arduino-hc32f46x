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

#include "variant.h"
#include "WVariant.h"

//
// ADC helpers
//

/**
 * @brief ADC config struct shorthand
 * @param channel ADC channel number. 0 == ADC1_IN0, ...
 * @note device is always ADC1_device
 */
#define ADC(ch)                               \
	{                                         \
		.device = &ADC1_device, .channel = ch \
	}

/**
 * @brief ADC config struct shorthand for no ADC function
 */
#define ADC_NONE                                   \
	{                                              \
		.device = NULL, .channel = ADC_PIN_INVALID \
	}

//
// TimerA helpers
//

/**
 * @brief TimerA config struct shorthand
 * @param uni TimerA unit. 1 == M4_TMRA1, ...
 * @param ch TimerA channel. 1 == TimeraCh1, ...
 * @param func TimerA GPIO function. 4 == Func_Tima0, 5 == Func_Tima1
 */
#define TIMA(uni, ch, func)                                      \
	{                                                            \
		.unit = uni, .channel = (ch - 1), .function = (func - 4) \
	}

/**
 * @brief TimerA config struct shorthand for no TimerA function
 */
#define TIMA_NONE                              \
	{                                          \
		.unit = 0, .channel = 0, .function = 0 \
	}

//
// Pin Map
//

extern const pin_info_t PIN_MAP[BOARD_NR_GPIO_PINS] = {
	// ---  PAx  ---
	{0, PortA, Pin00, ADC(ADC1_IN0), TIMA(2, 1, 4)},   // PA0
	{1, PortA, Pin01, ADC(ADC1_IN1), TIMA(2, 2, 4)},   // PA1
	{2, PortA, Pin02, ADC(ADC1_IN2), TIMA(5, 1, 5)},   // PA2
	{3, PortA, Pin03, ADC(ADC1_IN3), TIMA(5, 2, 5)},   // PA3
	{4, PortA, Pin04, ADC(ADC12_IN4), TIMA(3, 5, 5)},  // PA4
	{5, PortA, Pin05, ADC(ADC12_IN5), TIMA(3, 6, 5)},  // PA5
	{6, PortA, Pin06, ADC(ADC12_IN6), TIMA(3, 1, 5)},  // PA6
	{7, PortA, Pin07, ADC(ADC12_IN7), TIMA(3, 2, 5)},  // PA7
	{8, PortA, Pin08, ADC_NONE, TIMA(1, 1, 4)},		   // PA8
	{9, PortA, Pin09, ADC_NONE, TIMA(1, 2, 4)},		   // PA9
	{10, PortA, Pin10, ADC_NONE, TIMA(1, 3, 4)},	   // PA10
	{11, PortA, Pin11, ADC_NONE, TIMA(1, 4, 4)},	   // PA11
	{12, PortA, Pin12, ADC_NONE, TIMA(6, 1, 5)},	   // PA12
	{13, PortA, Pin13, ADC_NONE, TIMA(6, 2, 5)},	   // PA13
	{14, PortA, Pin14, ADC_NONE, TIMA(6, 3, 5)},	   // PA14
	{15, PortA, Pin15, ADC_NONE, TIMA(6, 4, 5)},	   // PA15
													   //
													   // ---  PBx  ---
													   //
	{0, PortB, Pin00, ADC(ADC12_IN8), TIMA(3, 3, 5)},  // PB0
	{1, PortB, Pin01, ADC(ADC12_IN9), TIMA(3, 4, 5)},  // PB1
	{2, PortB, Pin02, ADC_NONE, TIMA(1, 8, 4)},		   // PB2
	{3, PortB, Pin03, ADC_NONE, TIMA(6, 5, 5)},		   // PB3
	{4, PortB, Pin04, ADC_NONE, TIMA(6, 6, 5)},		   // PB4
	{5, PortB, Pin05, ADC_NONE, TIMA(6, 7, 5)},		   // PB5
	{6, PortB, Pin06, ADC_NONE, TIMA(6, 8, 5)},		   // PB6
	{7, PortB, Pin07, ADC_NONE, TIMA(4, 2, 4)},		   // PB7
	{8, PortB, Pin08, ADC_NONE, TIMA(4, 3, 4)},		   // PB8
	{9, PortB, Pin09, ADC_NONE, TIMA(4, 4, 4)},		   // PB9
	{10, PortB, Pin10, ADC_NONE, TIMA(5, 8, 5)},	   // PB10
	{11, PortB, Pin11, ADC_NONE, TIMA_NONE},		   // PB11
	{12, PortB, Pin12, ADC_NONE, TIMA(1, 8, 4)},	   // PB12
	{13, PortB, Pin13, ADC_NONE, TIMA(1, 5, 4)},	   // PB13
	{14, PortB, Pin14, ADC_NONE, TIMA(1, 6, 4)},	   // PB14
	{15, PortB, Pin15, ADC_NONE, TIMA(1, 7, 4)},	   // PB15
													   //
													   // ---  PCx  ---
													   //
	{0, PortC, Pin00, ADC(ADC12_IN10), TIMA(2, 5, 4)}, // PC0
	{1, PortC, Pin01, ADC(ADC12_IN11), TIMA(2, 6, 4)}, // PC1
	{2, PortC, Pin02, ADC(ADC1_IN12), TIMA(2, 7, 4)},  // PC2
	{3, PortC, Pin03, ADC(ADC1_IN13), TIMA(2, 8, 4)},  // PC3
	{4, PortC, Pin04, ADC(ADC1_IN14), TIMA(3, 7, 5)},  // PC4
	{5, PortC, Pin05, ADC(ADC1_IN15), TIMA(3, 8, 5)},  // PC5
	{6, PortC, Pin06, ADC_NONE, TIMA(5, 8, 5)},		   // PC6
	{7, PortC, Pin07, ADC_NONE, TIMA(5, 7, 5)},		   // PC7
	{8, PortC, Pin08, ADC_NONE, TIMA(5, 6, 5)},		   // PC8
	{9, PortC, Pin09, ADC_NONE, TIMA(5, 5, 5)},		   // PC9
	{10, PortC, Pin10, ADC_NONE, TIMA(5, 1, 5)},	   // PC10
	{11, PortC, Pin11, ADC_NONE, TIMA(5, 2, 5)},	   // PC11
	{12, PortC, Pin12, ADC_NONE, TIMA(5, 3, 5)},	   // PC12
	{13, PortC, Pin13, ADC_NONE, TIMA(4, 8, 4)},	   // PC13
	{14, PortC, Pin14, ADC_NONE, TIMA(4, 5, 4)},	   // PC14 XTAL32_OUT
	{15, PortC, Pin15, ADC_NONE, TIMA(4, 6, 4)},	   // PC15 XTAL32_IN
													   //
													   // ---  PDx  ---
													   //
	{0, PortD, Pin00, ADC_NONE, TIMA(5, 4, 5)},		   // PD0
	{1, PortD, Pin01, ADC_NONE, TIMA(6, 5, 5)},		   // PD1
	{2, PortD, Pin02, ADC_NONE, TIMA(6, 6, 5)},		   // PD2
	{3, PortD, Pin03, ADC_NONE, TIMA(6, 7, 5)},		   // PD3
	{4, PortD, Pin04, ADC_NONE, TIMA(6, 8, 5)},		   // PD4
	{5, PortD, Pin05, ADC_NONE, TIMA_NONE},			   // PD5
	{6, PortD, Pin06, ADC_NONE, TIMA_NONE},			   // PD6
	{7, PortD, Pin07, ADC_NONE, TIMA_NONE},			   // PD7
	{8, PortD, Pin08, ADC_NONE, TIMA(6, 1, 5)},		   // PD8
	{9, PortD, Pin09, ADC_NONE, TIMA(6, 2, 5)},		   // PD9
	{10, PortD, Pin10, ADC_NONE, TIMA(6, 3, 5)},	   // PD10
	{11, PortD, Pin11, ADC_NONE, TIMA(6, 4, 5)},	   // PD11
	{12, PortD, Pin12, ADC_NONE, TIMA(5, 5, 5)},	   // PD12
	{13, PortD, Pin13, ADC_NONE, TIMA(5, 6, 5)},	   // PD13
	{14, PortD, Pin14, ADC_NONE, TIMA(5, 7, 5)},	   // PD14
	{15, PortD, Pin15, ADC_NONE, TIMA(5, 8, 5)},	   // PD15
													   //
													   // ---  PEx  ---
													   //
	{0, PortE, Pin00, ADC_NONE, TIMA_NONE},			   // PE0
	{1, PortE, Pin01, ADC_NONE, TIMA_NONE},			   // PE1
	{2, PortE, Pin02, ADC_NONE, TIMA(3, 5, 4)},		   // PE2
	{3, PortE, Pin03, ADC_NONE, TIMA(3, 6, 4)},		   // PE3
	{4, PortE, Pin04, ADC_NONE, TIMA(3, 7, 4)},		   // PE4
	{5, PortE, Pin05, ADC_NONE, TIMA(3, 8, 4)},		   // PE5
	{6, PortE, Pin06, ADC_NONE, TIMA_NONE},			   // PE6
	{7, PortE, Pin07, ADC_NONE, TIMA_NONE},			   // PE7
	{8, PortE, Pin08, ADC_NONE, TIMA(1, 5, 4)},		   // PE8
	{9, PortE, Pin09, ADC_NONE, TIMA(1, 1, 4)},		   // PE9
	{10, PortE, Pin10, ADC_NONE, TIMA(1, 6, 4)},	   // PE10
	{11, PortE, Pin11, ADC_NONE, TIMA(1, 2, 4)},	   // PE11
	{12, PortE, Pin12, ADC_NONE, TIMA(1, 7, 4)},	   // PE12
	{13, PortE, Pin13, ADC_NONE, TIMA(1, 3, 4)},	   // PE13
	{14, PortE, Pin14, ADC_NONE, TIMA(1, 4, 4)},	   // PE14
	{15, PortE, Pin15, ADC_NONE, TIMA(1, 8, 4)},	   // PE15
													   //
													   // ---  PHx  ---
													   //
	{0, PortH, Pin00, ADC_NONE, TIMA(5, 3, 5)},		   // PH0   XTAL_IN
	{1, PortH, Pin01, ADC_NONE, TIMA(5, 4, 5)},		   // PH1   XTAL_OUT
	{2, PortH, Pin02, ADC_NONE, TIMA(4, 7, 4)},		   // PH2
};
