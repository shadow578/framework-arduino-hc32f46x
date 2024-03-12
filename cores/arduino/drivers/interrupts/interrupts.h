#ifndef __DYN_INTERRUPTS_H__
#define __DYN_INTERRUPTS_H__
#include <hc32_ddl.h>
#include <startup.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief vector table located in RAM, used by arduino core interrupt management
   * @note do not use this directly unless you know what you are doing
   */
  volatile vector_table_t ram_vector_table;

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
