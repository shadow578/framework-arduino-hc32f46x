#include "delay.h"
#include <hc32_ddl.h>

inline void delay(uint32_t dwMs)
{
    Ddl_Delay1ms(dwMs);
}

inline void delayMicroseconds(uint32_t dwUs)
{
    Ddl_Delay1us(dwUs);
}
