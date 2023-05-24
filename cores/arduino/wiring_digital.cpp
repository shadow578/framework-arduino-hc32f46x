#include "wiring_digital.h"
#include "drivers/gpio/gpio.h"
#include "wiring_constants.h"

void pinMode(uint32_t dwPin, uint32_t dwMode)
{
    if (dwPin >= BOARD_NR_GPIO_PINS)
    {
        return;
    }

    // build pin configuration
    stc_port_init_t pinConf;
    MEM_ZERO_STRUCT(pinConf);
    switch (dwMode)
    {
    case INPUT:
        pinConf.enPinMode = Pin_Mode_In;
        break;
    case INPUT_PULLUP:
        pinConf.enPinMode = Pin_Mode_In;
        pinConf.enPullUp = Enable;
        break;
    case INPUT_ANALOG:
        pinConf.enPinMode = Pin_Mode_Ana;
        break;
    case OUTPUT:
        pinConf.enPinMode = Pin_Mode_Out;
        break;
    default:
        return;
    }

    // set pind config
    PORT_InitGPIO(dwPin, &pinConf);
}

uint32_t getPinMode(uint32_t dwPin)
{
    if (dwPin >= BOARD_NR_GPIO_PINS)
    {
        return INPUT_FLOATING;
    }

    // read pin configuration
    stc_port_init_t pinConf;
    PORT_GetConfigGPIO(dwPin, &pinConf);
    switch (pinConf.enPinMode)
    {
    case Pin_Mode_Out:
        return OUTPUT;
    case Pin_Mode_In:
        return (pinConf.enPullUp == Enable) ? INPUT_PULLUP : INPUT;
    case Pin_Mode_Ana:
        return INPUT_ANALOG;
    default:
        return INPUT_FLOATING;
    }
}

void digitalWrite(uint32_t dwPin, uint32_t dwVal)
{
    if (dwPin >= BOARD_NR_GPIO_PINS)
    {
        return;
    }

    if (dwVal == HIGH)
    {
        PORT_SetBitsGPIO(dwPin);
    }
    else
    {
        PORT_ResetBitsGPIO(dwPin);
    }
}

int digitalRead(uint32_t ulPin)
{
    if (ulPin >= BOARD_NR_GPIO_PINS)
    {
        return LOW;
    }

    return PORT_GetBitGPIO(ulPin) ? HIGH : LOW;
}
