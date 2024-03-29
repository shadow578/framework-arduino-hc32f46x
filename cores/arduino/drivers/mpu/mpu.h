#pragma once
#include <stdint.h>

/**
 * @brief ARM Cortex-M4 MPU driver.
 */
namespace mpu
{
/**
 * @brief MPU region permissions.
 */
enum permissions_t
{
  /**
   * @brief disallow read and write access.
   */
  NO_ACCESS = 0,

  /**
   * @brief allow read access.
   */
  READ_ONLY = 1,

  /**
   * @brief allow read and write access.
   */
  READ_WRITE = 3
};

/**
 * @brief MPU region configuration.
 * https://interrupt.memfault.com/blog/fix-bugs-and-secure-firmware-with-the-mpu#mpu-region-base-address-register
 */
typedef struct region_config
{
  /**
   * @brief The base address of the region.
   * @note the base address must be aligned to the region size.
   * @note the lower 5 bits of the base address are masked, so minimum alignment is 32 bytes.
   */
  uint32_t base_address;

  /**
   * @brief The size of the region.
   * @note minimum size is 1
   * @note size in bytes is calculates using: 2^(size).
   * @example size = 8 -> 2^8 = 256 bytes.
   * @note MPU_RASR.SIZE = (size - 1)
   */
  uint8_t size;

  /**
   * @brief The access permissions of the region in privileged mode.
   */
  uint8_t access_permissions;

  /**
   * @brief Allow execution in the region (XN bit)?
   */
  bool allow_execute;
} region_config_t;

/**
 * @brief get mpu access permissions
 * @param privileged The privileged mode access permissions.
 * @param user The user mode access permissions.
 * @return The mpu access permissions (AP) register value
 */
constexpr uint8_t get_access_permissions(const permissions_t privileged, const permissions_t user)
{
  switch (privileged)
  {
  case permissions_t::NO_ACCESS:
    switch (user)
    {
    case permissions_t::NO_ACCESS:
      return 0; // AP[2:0] = 000
    case permissions_t::READ_ONLY:
    case permissions_t::READ_WRITE:
    default:
      break;
    }
    break;

  case permissions_t::READ_ONLY:
    switch (user)
    {
    case permissions_t::NO_ACCESS:
      return 5; // AP[2:0] = 101
    case permissions_t::READ_ONLY:
      return 6; // AP[2:0] = 110
    case permissions_t::READ_WRITE:
    default:
      break;
    }
    break;

  case permissions_t::READ_WRITE:
    switch (user)
    {
    case permissions_t::NO_ACCESS:
      return 1; // AP[2:0] = 001
    case permissions_t::READ_ONLY:
      return 2; // AP[2:0] = 010
    case permissions_t::READ_WRITE:
      return 3; // AP[2:0] = 011
    default:
      break;
    }
    break;
  }

  // unsupported access permission combination
  return 0xff;
}

/**
 * @brief The number of available MPU regions.
 */
constexpr uint8_t REGION_COUNT = 8;

/**
 * @brief Initialize the MPU.
 * @param enable Enable the MPU (ENABLE).
 * @param enable_for_privileged Enable the MPU for privileged mode (PRIVDEFENA).
 * @param enable_for_fault Enable the MPU for fault handling (HFNMIENA).
 */
void init(const bool enable = true, const bool enable_for_privileged = true, const bool enable_for_fault = false);

/**
 * @brief Enable a region in the MPU.
 * @param region The region number
 * @param config The region configuration.
 * @return true if the region was enabled successfully, false otherwise.
 */
bool enable_region(const uint8_t region, const region_config_t &config);

/**
 * @brief Disable a region in the MPU.
 * @param region The region number
 */
void disable_region(const uint8_t region);

/**
 * @brief Set the region enabled state without affecting the configuration.
 * @param region The region number
 * @param enabled The enabled state
 * @note this function can be used to temporarily disable a region without losing the configuration.
 */
void set_region_enabled(uint8_t region, bool enabled);
}; // namespace mpu
