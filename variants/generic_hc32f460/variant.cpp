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

extern const pin_info_t PIN_MAP[BOARD_NR_GPIO_PINS] = {
	// PAx
	{0, PortA, Pin00, {&ADC1_device, ADC1_IN0}},  // PA0
	{1, PortA, Pin01, {&ADC1_device, ADC1_IN1}},  // PA1
	{2, PortA, Pin02, {&ADC1_device, ADC1_IN2}},  // PA2
	{3, PortA, Pin03, {&ADC1_device, ADC1_IN3}},  // PA3
	{4, PortA, Pin04, {&ADC1_device, ADC12_IN4}}, // PA4
	{5, PortA, Pin05, {&ADC1_device, ADC12_IN5}}, // PA5
	{6, PortA, Pin06, {&ADC1_device, ADC12_IN6}}, // PA6
	{7, PortA, Pin07, {&ADC1_device, ADC12_IN7}}, // PA7
	{8, PortA, Pin08, {NULL, ADC_PIN_INVALID}},	  // PA8
	{9, PortA, Pin09, {NULL, ADC_PIN_INVALID}},	  // PA9

	{10, PortA, Pin10, {NULL, ADC_PIN_INVALID}}, // PA10
	{11, PortA, Pin11, {NULL, ADC_PIN_INVALID}}, // PA11
	{12, PortA, Pin12, {NULL, ADC_PIN_INVALID}}, // PA12
	{13, PortA, Pin13, {NULL, ADC_PIN_INVALID}}, // PA13
	{14, PortA, Pin14, {NULL, ADC_PIN_INVALID}}, // PA14
	{15, PortA, Pin15, {NULL, ADC_PIN_INVALID}}, // PA15

	// PBx
	{0, PortB, Pin00, {&ADC1_device, ADC12_IN8}}, // PB0
	{1, PortB, Pin01, {&ADC1_device, ADC12_IN9}}, // PB1
	{2, PortB, Pin02, {NULL, ADC_PIN_INVALID}},	  // PB2
	{3, PortB, Pin03, {NULL, ADC_PIN_INVALID}},	  // PB3
	{4, PortB, Pin04, {NULL, ADC_PIN_INVALID}},	  // PB4
	{5, PortB, Pin05, {NULL, ADC_PIN_INVALID}},	  // PB5
	{6, PortB, Pin06, {NULL, ADC_PIN_INVALID}},	  // PB6
	{7, PortB, Pin07, {NULL, ADC_PIN_INVALID}},	  // PB7
	{8, PortB, Pin08, {NULL, ADC_PIN_INVALID}},	  // PB8
	{9, PortB, Pin09, {NULL, ADC_PIN_INVALID}},	  // PB9
	{10, PortB, Pin10, {NULL, ADC_PIN_INVALID}},  // PB10
	{11, PortB, Pin11, {NULL, ADC_PIN_INVALID}},  // PB11
	{12, PortB, Pin12, {NULL, ADC_PIN_INVALID}},  // PB12
	{13, PortB, Pin13, {NULL, ADC_PIN_INVALID}},  // PB13
	{14, PortB, Pin14, {NULL, ADC_PIN_INVALID}},  // PB14
	{15, PortB, Pin15, {NULL, ADC_PIN_INVALID}},  // PB15

	// PCx
	{0, PortC, Pin00, {&ADC1_device, ADC12_IN10}}, // PC0
	{1, PortC, Pin01, {&ADC1_device, ADC12_IN11}}, // PC1
	{2, PortC, Pin02, {&ADC1_device, ADC1_IN12}},  // PC2
	{3, PortC, Pin03, {&ADC1_device, ADC1_IN13}},  // PC3
	{4, PortC, Pin04, {&ADC1_device, ADC1_IN14}},  // PC4
	{5, PortC, Pin05, {&ADC1_device, ADC1_IN15}},  // PC5
	{6, PortC, Pin06, {NULL, ADC_PIN_INVALID}},	   // PC6
	{7, PortC, Pin07, {NULL, ADC_PIN_INVALID}},	   // PC7
	{8, PortC, Pin08, {NULL, ADC_PIN_INVALID}},	   // PC8
	{9, PortC, Pin09, {NULL, ADC_PIN_INVALID}},	   // PC9
	{10, PortC, Pin10, {NULL, ADC_PIN_INVALID}},   // PC10
	{11, PortC, Pin11, {NULL, ADC_PIN_INVALID}},   // PC11
	{12, PortC, Pin12, {NULL, ADC_PIN_INVALID}},   // PC12
	{13, PortC, Pin13, {NULL, ADC_PIN_INVALID}},   // PC13
	{14, PortC, Pin14, {NULL, ADC_PIN_INVALID}},   // PC14 XTAL32_OUT
	{15, PortC, Pin15, {NULL, ADC_PIN_INVALID}},   // PC15 XTAL32_IN

	// PDx
	{0, PortD, Pin00, {NULL, ADC_PIN_INVALID}},	 // PD0
	{1, PortD, Pin01, {NULL, ADC_PIN_INVALID}},	 // PD1
	{2, PortD, Pin02, {NULL, ADC_PIN_INVALID}},	 // PD2
	{3, PortD, Pin03, {NULL, ADC_PIN_INVALID}},	 // PD3
	{4, PortD, Pin04, {NULL, ADC_PIN_INVALID}},	 // PD4
	{5, PortD, Pin05, {NULL, ADC_PIN_INVALID}},	 // PD5
	{6, PortD, Pin06, {NULL, ADC_PIN_INVALID}},	 // PD6
	{7, PortD, Pin07, {NULL, ADC_PIN_INVALID}},	 // PD7
	{8, PortD, Pin08, {NULL, ADC_PIN_INVALID}},	 // PD8
	{9, PortD, Pin09, {NULL, ADC_PIN_INVALID}},	 // PD9
	{10, PortD, Pin10, {NULL, ADC_PIN_INVALID}}, // PD10
	{11, PortD, Pin11, {NULL, ADC_PIN_INVALID}}, // PD11
	{12, PortD, Pin12, {NULL, ADC_PIN_INVALID}}, // PD12
	{13, PortD, Pin13, {NULL, ADC_PIN_INVALID}}, // PD13
	{14, PortD, Pin14, {NULL, ADC_PIN_INVALID}}, // PD14
	{15, PortD, Pin15, {NULL, ADC_PIN_INVALID}}, // PD15

	// PEx
	{0, PortE, Pin00, {NULL, ADC_PIN_INVALID}},	 // PE0
	{1, PortE, Pin01, {NULL, ADC_PIN_INVALID}},	 // PE1
	{2, PortE, Pin02, {NULL, ADC_PIN_INVALID}},	 // PE2
	{3, PortE, Pin03, {NULL, ADC_PIN_INVALID}},	 // PE3
	{4, PortE, Pin04, {NULL, ADC_PIN_INVALID}},	 // PE4
	{5, PortE, Pin05, {NULL, ADC_PIN_INVALID}},	 // PE5
	{6, PortE, Pin06, {NULL, ADC_PIN_INVALID}},	 // PE6
	{7, PortE, Pin07, {NULL, ADC_PIN_INVALID}},	 // PE7
	{8, PortE, Pin08, {NULL, ADC_PIN_INVALID}},	 // PE8
	{9, PortE, Pin09, {NULL, ADC_PIN_INVALID}},	 // PE9
	{10, PortE, Pin10, {NULL, ADC_PIN_INVALID}}, // PE10
	{11, PortE, Pin11, {NULL, ADC_PIN_INVALID}}, // PE11
	{12, PortE, Pin12, {NULL, ADC_PIN_INVALID}}, // PE12
	{13, PortE, Pin13, {NULL, ADC_PIN_INVALID}}, // PE13
	{14, PortE, Pin14, {NULL, ADC_PIN_INVALID}}, // PE14
	{15, PortE, Pin15, {NULL, ADC_PIN_INVALID}}, // PE15

	// PHx
	{0, PortH, Pin00, {NULL, ADC_PIN_INVALID}}, // PH0   XTAL_IN
	{1, PortH, Pin01, {NULL, ADC_PIN_INVALID}}, // PH1   XTAL_OUT
	{2, PortH, Pin02, {NULL, ADC_PIN_INVALID}}, // PH2
};
