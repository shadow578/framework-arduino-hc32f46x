#pragma once
#include <hc32_ddl.h>

/**
 * @brief i2c peripheral config
 */
typedef struct i2c_peripheral_config_t
{
  /**
   * @brief The base address of the i2c peripheral.
   */
  M4_I2C_TypeDef *register_base;

  /**
   * @brief The clock id of the i2c peripheral.
   * @note in FCG1
   */
  const uint32_t clock_id;

  /**
   * @brief The SDA pin function.
   */
  const en_port_func_t sda_func;

  /**
   * @brief The SCL pin function.
   */
  const en_port_func_t scl_func;

} i2c_peripheral_config_t;

/**
 * @brief i2c device config
 */
typedef struct i2c_device_config_t
{
  /**
   * @brief The peripheral config of the i2c.
   */
  const i2c_peripheral_config_t peripheral;

} i2c_device_config_t;

/**
 * I2C unit 1
 */
extern const i2c_device_config_t I2C1_config;

/**
 * I2C unit 2
 */
extern const i2c_device_config_t I2C2_config;

/**
 * I2C unit 3
 */
extern const i2c_device_config_t I2C3_config;
