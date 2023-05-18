#include "delay.h"
#include <hc32_ddl.h>

void delay(uint32_t dwMs)
{
    Ddl_Delay1ms(dwMs);
}

void delayMicroseconds(uint32_t dwUs)
{
    Ddl_Delay1us(dwUs);
}
