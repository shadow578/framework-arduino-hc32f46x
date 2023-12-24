#include "i2c_config.h"

const i2c_device_config_t I2C1_config = {
    .peripheral =
        {
            .register_base = M4_I2C1,
            .clock_id = PWC_FCG1_PERIPH_I2C1,
            .sda_func = Func_I2c1_Sda,
            .scl_func = Func_I2c1_Scl,
        },
};

const i2c_device_config_t I2C2_config = {
    .peripheral =
        {
            .register_base = M4_I2C2,
            .clock_id = PWC_FCG1_PERIPH_I2C2,
            .sda_func = Func_I2c2_Sda,
            .scl_func = Func_I2c2_Scl,
        },
};

const i2c_device_config_t I2C3_config = {
    .peripheral =
        {
            .register_base = M4_I2C3,
            .clock_id = PWC_FCG1_PERIPH_I2C3,
            .sda_func = Func_I2c3_Sda,
            .scl_func = Func_I2c3_Scl,
        },
};