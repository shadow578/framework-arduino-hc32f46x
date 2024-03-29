#include "mpu.h"
#include "../../core_debug.h"
#include <hc32_ddl.h>

#if !__MPU_PRESENT
  // all devices in the HC32Fxxx series should have an MPU
  #error "MPU is not available for this device?!"
#endif

constexpr uint32_t FLASH_REGION_START = 0x00000000;
constexpr uint32_t SRAM_REGION_START = 0x1FFF8000;
constexpr uint32_t PERIPH_REGION_START = 0x40008000;
constexpr uint32_t PERIPH_REGION_END = 0xFFFFFFFF;

/**
 * @brief Get the region type based on the address.
 * @param address The address to check.
 * @return The region type. 0 = FLASH, 1 = internal SRAM, 2 = external SRAM, 3 = PERIPHERALS.
 */
inline uint8_t get_region_type(const uint32_t address)
{
  if (/*address >= FLASH_REGION_START &&*/ address < SRAM_REGION_START)
  {
    return 0; // FLASH
  }
  else if (/*address >= SRAM_REGION_START &&*/ address < PERIPH_REGION_START)
  {
    return 1; // SRAM
  }
  else /*if (address >= PERIPH_REGION_START && address <= PERIPH_REGION_END)*/
  {
    return 3; // PERIPHERALS
  }
}

/**
 * @brief enter critical section for MPU configuration.
 * @param fn The function to execute in the critical section.
 * @note no interrupts and disable mpu
 */
#define MPU_CRITICAL_SECTION(fn)                                                                                       \
  {                                                                                                                    \
    __disable_irq();                                                                                                   \
    uint32_t mpu_ctrl = MPU->CTRL;                                                                                     \
    MPU->CTRL = mpu_ctrl & ~MPU_CTRL_ENABLE_Msk;                                                                       \
    __DSB();                                                                                                           \
    {                                                                                                                  \
      fn;                                                                                                              \
    }                                                                                                                  \
    __DSB();                                                                                                           \
    MPU->CTRL = mpu_ctrl;                                                                                              \
    __enable_irq();                                                                                                    \
  }

namespace mpu
{
void init(const bool enable, const bool enable_for_privileged, const bool enable_for_fault)
{
  // validate that the REGION_COUNT is not greater than the hardware limit
  const uint8_t hw_region_count = (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos;
  CORE_ASSERT(REGION_COUNT <= hw_region_count, "MPU region count exceeds hardware limit!");
  CORE_DEBUG_PRINTF("MPU: %d regions available, %d regions usable\n", hw_region_count, REGION_COUNT);

  // enable the MPU
  uint32_t mpu_ctrl = 0;

  if (enable)
  {
    mpu_ctrl |= MPU_CTRL_ENABLE_Msk;
  }

  if (enable_for_privileged)
  {
    mpu_ctrl |= MPU_CTRL_PRIVDEFENA_Msk;
  }

  if (enable_for_fault)
  {
    mpu_ctrl |= MPU_CTRL_HFNMIENA_Msk;
  }

  CORE_DEBUG_PRINTF("MPU init (CTRL: 0x%08lx)\n", mpu_ctrl);
  MPU->CTRL = mpu_ctrl;
}

bool enable_region(const uint8_t region, const region_config_t &config)
{
  // validate the region number
  CORE_ASSERT(region < REGION_COUNT, "region number out of bounds!", return false);
  CORE_ASSERT(region == (region & MPU_RBAR_REGION_Msk), "region number masking failed!", return false);

  // validate size and access permissions
  CORE_ASSERT(config.size > 0, "region size must be greater than 0!", return false);
  CORE_ASSERT(config.access_permissions != 0xff, "unsupported access permissions!", return false);

  // validate the base address alignment
  uint32_t base_address = config.base_address & MPU_RBAR_ADDR_Msk;
  CORE_ASSERT(base_address == config.base_address, "base address not aligned!", return false);
  CORE_ASSERT((base_address & ((1 << (config.size)) - 1)) == 0, "base address not aligned to region size!",
              return false);

  // get [T]ype [E]xtension [M]ask and
  // [S]hareable, [C]acheable and [B]ufferable bits
  // based on the region type
  uint32_t tex_s_c_b = 0;
  switch (get_region_type(base_address))
  {
  case 0:
    // FLASH
    // normal memory, non-shareable, write-through
    // TEX = 0b000, C = 1, B = 0, S = 0
    tex_s_c_b = 0x2;
    break;
  case 1:
    // SRAM
    // normal memory, shareable, write-through
    // TEX = 0b000, C = 1, B = 0, S = 1
    tex_s_c_b = 0x6;
    break;
  // case 2:
  //   // EXTERNAL SRAM (not supported yet!)
  //   // normal memory, shareable, write-back, write-allocate
  //   // TEX = 0b000, C = 1, B = 1, S = 1
  //   tex_s_c_b = 0x7;
  //   break;
  case 3:
    // PERIPHERALS
    // device memory, non-cacheable, non-bufferable
    // TEX = 0b000, C = 0, B = 1, S = 1
    tex_s_c_b = 0x5;
    break;
  }
  tex_s_c_b <<= MPU_RASR_B_Pos;
  CORE_ASSERT(tex_s_c_b == (tex_s_c_b & (MPU_RASR_TEX_Msk | MPU_RASR_S_Msk | MPU_RASR_C_Msk | MPU_RASR_B_Msk)),
              "invalid TEX / S / C / B bits!", return false);

  // prepare, then set the RBAR and RASR registers
  // note that RASR->SRD is not used here
  uint32_t rbar = base_address | MPU_RBAR_VALID_Msk | region;
  uint32_t rasr = (config.allow_execute ? 0 : MPU_RASR_XN_Msk)     // allow execution (e[X]ecute [N]ever)?
                  | (config.access_permissions << MPU_RASR_AP_Pos) // [A]ccess [P]ermissions
                  | tex_s_c_b                                      // TEX + S + C + B
                  | ((config.size - 1) << MPU_RASR_SIZE_Pos)       // region size
                  | MPU_RASR_ENABLE_Msk;                           // enable the region

  CORE_DEBUG_PRINTF("MPU: enabling region %d (RBAR: 0x%08lx; RASR: 0x%08lx)\n", region, rbar, rasr);
  MPU_CRITICAL_SECTION({
    MPU->RBAR = rbar;
    MPU->RASR = rasr;
  });
  return true;
}

void disable_region(const uint8_t region)
{
  // validate the region number
  CORE_ASSERT(region < REGION_COUNT, "region number out of bounds!", return);
  CORE_ASSERT(region == (region & MPU_RBAR_REGION_Msk), "region number masking failed!", return);
  CORE_DEBUG_PRINTF("MPU: disabling region %d\n", region);

  // clear the RBAR and RASR registers
  MPU_CRITICAL_SECTION({
    MPU->RBAR = MPU_RBAR_VALID_Msk | region;
    MPU->RASR = 0;
  });
}

void set_region_enabled(uint8_t region, bool enabled)
{
  // validate the region number
  CORE_ASSERT(region < REGION_COUNT, "region number out of bounds!", return);
  CORE_ASSERT(region == (region & MPU_RBAR_REGION_Msk), "region number masking failed!", return);
  CORE_DEBUG_PRINTF("MPU: set region %d enable to %d\n", region, enabled);

  MPU_CRITICAL_SECTION({
    MPU->RNR = region & MPU_RNR_REGION_Msk;

    if (enabled)
    {
      MPU->RASR |= MPU_RASR_ENABLE_Msk;
    }
    else
    {
      MPU->RASR &= ~MPU_RASR_ENABLE_Msk;
    }
  });
}
} // namespace mpu
