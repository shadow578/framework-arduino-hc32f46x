#ifndef __DYN_INTERRUPTS_H__
#define __DYN_INTERRUPTS_H__
#include <hc32_ddl.h>
#include <startup.h>
#include "../irqn/irqn.h"

/**
 * @brief number of usable IRQ#n
 * @note only IRQn 0-127 can be used by the driver
 */
#define USEABLE_IRQ_COUNT 128

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief initialize dynamic vector table
   * @note called by arduino core init
   */
  void interrupts_init(void);

  /**
   * @brief register interrupt handler for specified source
   * @param source interrupt source
   * @param handler interrupt handler
   * @return IRQn that was assigned. required for interrupt resign. -1 if failed
   * @note automatically assigns the handler to the next available IRQn
   */
  int interrupt_register(const en_int_src_t source, func_ptr_t handler);

  /**
   * @brief resign interrupt handler for specified IRQn
   * @param irqn IRQn to resign
   * @return true if successful, false if failed
   */
  bool interrupt_resign(const int irqn);

#ifdef __cplusplus
}
#endif
#endif // __DYN_INTERRUPTS_H__
