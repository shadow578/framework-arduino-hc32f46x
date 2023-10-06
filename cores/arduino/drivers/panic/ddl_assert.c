#include <hc32_ddl.h>
#include "panic.h"

void Ddl_AssertHandler(uint8_t *file, int16_t line)
{
    panic_begin();
    panic_printf("DDL assert failure in file %s, line %d\n", file, line);
    panic_end();
}
