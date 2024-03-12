#include "interrupts.h"
#include "../../core_debug.h"
#include <algorithm>

/**
 * @note
 * required to be aligned to the boundary width of the lowest power
 * of 2 that will fit the entire vector table.
 * since the vector table takes [(144 + 16) * 4 =] 640 bytes, the
 * lowest power of 2 that will fit the entire vector table is 1024.
 */
__attribute__((aligned(1024))) volatile vector_table_t ram_vector_table;

/**
 * @brief no handler function
 */
#define no_handler (&__default_handler)

void interrupts_init()
{
  // copy the current vector table to RAM
  vector_table_t *current_vt = (vector_table_t *)SCB->VTOR;
  vector_table_t *target_vt = &ram_vector_table;
  std::copy(current_vt, current_vt + sizeof(vector_table_t), target_vt);

  // check the copy was successful
  CORE_ASSERT(std::equal(current_vt, current_vt + sizeof(vector_table_t), target_vt), "vector table copy failed!",
              return);

  // check the target vector table address is valid:
  // - bits [8:0] of the table offset are always zero
  CORE_ASSERT(((uint32_t)target_vt & 0x1ff) == 0, "invalid vector table address", return);

  // relocate the vector table to RAM
  // this is done with interrupts disabled and some special memory barriers
  __disable_irq();

  SCB->VTOR = (uint32_t)target_vt;
  __DSB();

  __enable_irq();
}

int interrupt_register(const en_int_src_t source, func_ptr_t handler)
{
}

bool interrupt_resign(const int irqn)
{
}
