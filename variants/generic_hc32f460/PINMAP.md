| Name | ADC        | TimerA (func4) | TimerA (func5) | Special Function |
| ---- | ---------- | -------------- | -------------- | ---------------- |
| PA0  | ADC1_IN0   | TIMA2_PWM1     |
| PA1  | ADC1_IN1   | TIMA2_PWM2     |
| PA2  | ADC1_IN2   | TIMA2_PWM3     | TIMA5_PWM1     |
| PA3  | ADC1_IN3   | TIMA2_PWM4     | TIMA5_PWM2     |
| PA4  | ADC12_IN4  |                | TIMA3_PWM5     |
| PA5  | ADC12_IN5  | TIMA2_PWM1     | TIMA3_PWM6     |
| PA6  | ADC12_IN6  |                | TIMA3_PWM1     |
| PA7  | ADC12_IN7  | TIMA1_PWM5     | TIMA3_PWM2     |
| PA8  |            | TIMA1_PWM1     |
| PA9  |            | TIMA1_PWM2     |
| PA10 |            | TIMA1_PWM3     |
| PA11 |            | TIMA1_PWM4     |
| PA12 |            |                | TIMA6_PWM1     |
| PA13 |            | TIMA1_PWM5     | TIMA6_PWM2     |
| PA14 |            | TIMA2_PWM6     | TIMA6_PWM3     |
| PA15 |            | TIMA2_PWM1     | TIMA6_PWM4     |
| PB0  | ADC12_IN8  | TIMA1_PWM6     | TIMA3_PWM3     |
| PB1  | ADC12_IN9  | TIMA1_PWM7     | TIMA3_PWM4     |
| PB2  |            | TIMA1_PWM8     |
| PB3  |            | TIMA2_PWM2     | TIMA6_PWM5     |
| PB4  |            | TIMA3_PWM1     | TIMA6_PWM6     |
| PB5  |            | TIMA3_PWM2     | TIMA6_PWM7     |
| PB6  |            | TIMA4_PWM1     | TIMA6_PWM8     |
| PB7  |            | TIMA4_PWM2     |
| PB8  |            | TIMA4_PWM3     |
| PB9  |            | TIMA4_PWM4     |
| PB10 |            | TIMA2_PWM3     | TIMA5_PWM8     |
| PB11 |            |
| PB12 |            | TIMA1_PWM8     |
| PB13 |            | TIMA1_PWM5     |
| PB14 |            | TIMA1_PWM6     |
| PB15 |            | TIMA1_PWM7     |
| PC0  | ADC12_IN10 | TIMA2_PWM5     |
| PC1  | ADC12_IN11 | TIMA2_PWM6     |
| PC2  | ADC1_IN12  | TIMA2_PWM7     |
| PC3  | ADC1_IN13  | TIMA2_PWM8     |
| PC4  | ADC1_IN14  |                | TIMA3_PWM7     |
| PC5  | ADC1_IN15  |                | TIMA3_PWM8     |
| PC6  |            | TIMA3_PWM1     | TIMA5_PWM8     |
| PC7  |            | TIMA3_PWM2     | TIMA5_PWM7     |
| PC8  |            | TIMA3_PWM3     | TIMA5_PWM6     |
| PC9  |            | TIMA3_PWM4     | TIMA5_PWM5     |
| PC10 |            | TIMA2_PWM7     | TIMA5_PWM1     |
| PC11 |            | TIMA2_PWM8     | TIMA5_PWM2     |
| PC12 |            |                | TIMA5_PWM3     |
| PC13 |            | TIMA4_PWM8     |
| PC14 |            | TIMA4_PWM5     |                | XTAL32_OUT       |
| PC15 |            | TIMA4_PWM6     |                | XTAL32_IN        |
| PD0  |            |                | TIMA5_PWM4     |
| PD1  |            |                | TIMA6_PWM5     |
| PD2  |            | TIMA2_PWM4     | TIMA6_PWM6     |
| PD3  |            |                | TIMA6_PWM7     |
| PD4  |            |                | TIMA6_PWM8     |
| PD5  |            |
| PD6  |            |
| PD7  |            |
| PD8  |            |                | TIMA6_PWM1     |
| PD9  |            |                | TIMA6_PWM2     |
| PD10 |            |                | TIMA6_PWM3     |
| PD11 |            |                | TIMA6_PWM4     |
| PD12 |            | TIMA4_PWM1     | TIMA5_PWM5     |
| PD13 |            | TIMA4_PWM2     | TIMA5_PWM6     |
| PD14 |            | TIMA4_PWM3     | TIMA5_PWM7     |
| PD15 |            | TIMA4_PWM4     | TIMA5_PWM8     |
| PE0  |            |
| PE1  |            |
| PE2  |            | TIMA3_PWM5     |
| PE3  |            | TIMA3_PWM6     |
| PE4  |            | TIMA3_PWM7     |
| PE5  |            | TIMA3_PWM8     |
| PE6  |            |
| PE7  |            |
| PE8  |            | TIMA1_PWM5     |
| PE9  |            | TIMA1_PWM1     |
| PE10 |            | TIMA1_PWM6     |
| PE11 |            | TIMA1_PWM2     |
| PE12 |            | TIMA1_PWM7     |
| PE13 |            | TIMA1_PWM3     |
| PE14 |            | TIMA1_PWM4     |
| PE15 |            | TIMA1_PWM8     |
| PH0  |            |                | TIMA5_PWM3     | XTAL_IN          |
| PH1  |            |                | TIMA5_PWM4     | XTAL_OUT         |
| PH2  |            | TIMA4_PWM7     |

> Note: `ADC1_IN2` refers to ADC1 Channel 2. `ADC12_IN10` refers to ADC1 or ADC2 Channel 10, but only ADC1 is used.

> Note: `TIMA2_PWM3` refers to TimerA Unit 2, Channel 3. `func4` and `func5` refer to GPIO function. `func6` is not listed, as it does not concern PWM output.

> Note: Arduino core will use TimerA func5 where available, and func4 otherwise.
