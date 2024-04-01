#include "interrupts.h"
#include "../../core_debug.h"
#include <algorithm>

#if !defined(PROTECT_VECTOR_TABLE)
  #define PROTECT_VECTOR_TABLE 1
#endif

#if PROTECT_VECTOR_TABLE
  #include "../mpu/mpu.h"

/**
 * vector table MPU region
 */
constexpr uint8_t VT_MPU_REGION = 0;

  /**
   * @brief allow ram vector table to be written to during this section
   * @param fn code block
   */
  #define RAM_VT_ALLOW_WRITE(fn)                                                                                       \
    {                                                                                                                  \
      mpu::set_region_enabled(VT_MPU_REGION, false);                                                                   \
      fn;                                                                                                              \
      mpu::set_region_enabled(VT_MPU_REGION, true);                                                                    \
    }
#else
  #define RAM_VT_ALLOW_WRITE(fn) fn
#endif // PROTECT_VECTOR_TABLE

/**
 * @brief vector table with (optional) padding to 1024 bytes
 */
struct ram_vector_table_t : vector_table_t
{
#if PROTECT_VECTOR_TABLE
  uint8_t _mpu_padding[1024 - sizeof(vector_table_t)];
#endif
};

static_assert(sizeof(ram_vector_table_t) == (PROTECT_VECTOR_TABLE ? 1024 : sizeof(vector_table_t)),
              "invalid RAM vector table size");

/**
 * @note
 * required to be aligned to the boundary width of the lowest power
 * of 2 that will fit the entire vector table.
 * since the vector table takes [(144 + 16) * 4 =] 640 bytes, the
 * lowest power of 2 that will fit the entire vector table is 1024.
 */
__attribute__((aligned(1024))) volatile ram_vector_table_t ram_vector_table;

void __on_default_handler()
{
  panic("__default_handler called");
  while (true)
    ;
}

/**
 * @brief default value for interrupt source selection register
 */
constexpr uint32_t INTSEL_DEFAULT = 0x1FFu;

/**
 * @brief default irq handler
 */
#define no_handler (&__default_handler)

/**
 * @brief get the interrupt selection register for the specified IRQn
 * @param irqn IRQ#n
 * @return pointer to the interrupt selection register
 * @note only valid for IRQ#0-127
 */
static inline volatile stc_intc_sel_field_t *get_interrupt_selection_register(const int irqn)
{
  return reinterpret_cast<volatile stc_intc_sel_field_t *>(reinterpret_cast<uint32_t>(&M4_INTC->SEL0) +
                                                           (sizeof(stc_intc_sel_field_t) * irqn));
}

/**
 * @brief check if the specified source is valid for the specified IRQn
 * @param irqn IRQ#n
 * @param source interrupt source
 * @return true if the source is valid for the IRQn, false otherwise
 * @note only supports IRQ#0~127
 */
static bool is_valid_source_for_irqn(const int irqn, const en_int_src_t source)
{
  // IRQ < 0 and IRQ > 128 are invalid for the driver
  // IRQ#128-144 only support shared interrupts using a bit mask, so we don't support them
  if (irqn < 0 || (irqn >= 128 /*&& irqn <= 144*/))
  {
    return false;
  }

  // IRQ#32~127 have a limited selection
  if (irqn >= 32 && irqn <= 127)
  {
    // see reference manual, section 10.3.2 "Interrupt Event Request Sequence Number"
    // and table 10-2 "Interrupt Event Request Sequence Number and Selection", column "NVIC Vector 32~127"
    // for details on what sources can be selected for IRQ#32~127
    return (((source / 32) * 6 + 32) <= irqn) && (((source / 32) * 6 + 37) >= irqn);
  }

  // IRQ#0~31 can select all sources
  return true;
}

void interrupts_init()
{
  // copy the current vector table to RAM
  vector_table_t *current_vt = (vector_table_t *)SCB->VTOR;
  vector_table_t *target_vt = (vector_table_t *)&ram_vector_table;
  memcpy(target_vt, current_vt, sizeof(vector_table_t));

  // check the copy was successful
  CORE_ASSERT(memcmp(target_vt, current_vt, sizeof(vector_table_t)) == 0, "vector table copy failed!", return);

  // check the target vector table address is valid:
  // - bits [8:0] of the table offset are always zero
  CORE_ASSERT(((uint32_t)target_vt & 0x1ff) == 0, "invalid vector table address", return);

  // relocate the vector table to RAM
  // this is done with interrupts disabled and some special memory barriers
  __disable_irq();

  SCB->VTOR = (uint32_t)target_vt;
  __DSB();

  __enable_irq();

  // assert the VTOR was actually updated
  // this assertion should always panic, even when core debug is disabled
  CORE_ASSERT(SCB->VTOR == (uint32_t)target_vt, "failed to update VTOR", panic(""))

#if PROTECT_VECTOR_TABLE
  // protect the vector table using the MPU
  mpu::init();
  mpu::enable_region(VT_MPU_REGION, //
                     {
                         .base_address = (uint32_t)target_vt,
                         .size = 10, // 1024 bytes
                         .access_permissions = mpu::get_access_permissions(mpu::READ_ONLY, mpu::READ_ONLY),
                         .allow_execute = false,
                     });
#endif
}

int interrupt_register(const en_int_src_t source, func_ptr_t handler)
{
  CORE_ASSERT(handler != NULL, "handler is NULL", return -1);

  // get next free IRQ#n
  IRQn_Type irqn;
  CORE_ASSERT(irqn_auto_assign_ex(irqn, source), "no free IRQn available", return -1);

  // register the interrupt
  stc_irq_regi_conf_t irqRegiConf = {
      .enIntSrc = source,
      .enIRQn = irqn,
      .pfnCallback = handler,
  };
  CORE_ASSERT(enIrqRegistration(&irqRegiConf) == Ok, "enIrqRegistration failed", return -1);
  return static_cast<int>(irqn);
}

bool interrupt_resign(const int irqn)
{
  CORE_ASSERT(irqn >= 0 && irqn < USEABLE_IRQ_COUNT, "IRQn out of range", return false);

  IRQn_Type iirqn = static_cast<IRQn_Type>(irqn);
  enIrqResign(iirqn);
  _irqn_aa_resign(iirqn);
  return true;
}

bool irqn_auto_assign_ex(IRQn_Type &irqn, const en_int_src_t source)
{
  // find the next irq handler #n that is not already assigned
  // where n is the irq number and 0 <= n <= USEABLE_IRQ_COUNT
  for (int i = 0; i < USEABLE_IRQ_COUNT; i++)
  {
    if (ram_vector_table.irqs[i] == no_handler)
    {
      // this IRQ#n is free, check it can be used for the specified source
      // skip the check if the source is INT_MAX
      if (source == INT_MAX || is_valid_source_for_irqn(i, source))
      {
        irqn = static_cast<IRQn_Type>(i);
        return true;
      }
    }
  }

  // no irq available
  return false;
}

//
// compatibility layer for the old interrupt API
//

en_result_t enIrqRegistration(const stc_irq_regi_conf_t *pstcIrqRegiConf)
{
  CORE_ASSERT(pstcIrqRegiConf != NULL, "pstcIrqRegiConf is NULL", return ErrorInvalidParameter);
  CORE_ASSERT(pstcIrqRegiConf->pfnCallback != NULL, "pstcIrqRegiConf->pfnCallback is NULL",
              return ErrorInvalidParameter);

  const int irqn = static_cast<int>(pstcIrqRegiConf->enIRQn);
  CORE_ASSERT(irqn >= 0 && irqn < USEABLE_IRQ_COUNT, "IRQn out of range", return ErrorInvalidParameter);

  // check if the source is valid for the IRQn
  const en_int_src_t source = pstcIrqRegiConf->enIntSrc;
  CORE_ASSERT(is_valid_source_for_irqn(irqn, source), "invalid source selection for IRQn",
              return ErrorInvalidParameter);

  // get the interrupt source selection register
  auto intSel = get_interrupt_selection_register(irqn);

  // interrupt selection register should be the default value
  CORE_ASSERT(intSel->INTSEL == INTSEL_DEFAULT, "INTSEL is not at reset value", return ErrorUninitialized);

  // set the interrupt source selection
  intSel->INTSEL = source;

  // set the handler in the vector table
  CORE_ASSERT(ram_vector_table.irqs[irqn] == no_handler, "IRQn handler already assigned", return ErrorUninitialized);

  RAM_VT_ALLOW_WRITE({ ram_vector_table.irqs[irqn] = pstcIrqRegiConf->pfnCallback; });
  return Ok;
}

en_result_t enIrqResign(IRQn_Type enIRQn)
{
  int irqn = static_cast<int>(enIRQn);
  CORE_ASSERT(irqn >= 0 && irqn < USEABLE_IRQ_COUNT, "IRQn out of range", return ErrorInvalidParameter);

  // get the interrupt source selection register
  auto intSel = get_interrupt_selection_register(irqn);

  // interrupt selection register should NOT be the default value
  // if it is, no interrupt source was configured for this IRQn and we print a warning
  if (intSel->INTSEL == INTSEL_DEFAULT)
  {
    CORE_DEBUG_PRINTF("cannot resign IRQ#%d: INTSEL already reset\n", irqn);
  }
  intSel->INTSEL = INTSEL_DEFAULT;

  // reset the handler in the vector table
  // if the handler is already no_handler, we print a warning
  if (ram_vector_table.irqs[irqn] == no_handler)
  {
    CORE_DEBUG_PRINTF("cannot resign IRQ#%d: handler already no_handler\n", irqn);
  }

  RAM_VT_ALLOW_WRITE({ ram_vector_table.irqs[irqn] = no_handler; });

  return Ok;
}

en_result_t _irqn_aa_get(IRQn_Type &irqn)
{
  return irqn_auto_assign_ex(irqn, INT_MAX) ? Ok : Error;
}

en_result_t _irqn_aa_resign(IRQn_Type &irqn)
{
  // do nothing since resigning the interrupt already frees the IRQn
  // only check that the interrupt was actually resigned before calling this function
  CORE_ASSERT(ram_vector_table.irqs[irqn] != no_handler, "IRQ was not resigned before auto-assign resignment",
              return Error);
  return Ok;
}
