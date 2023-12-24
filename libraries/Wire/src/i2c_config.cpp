#include "i2c_config.h"

i2c_device_config_t I2C1_config = {
    .peripheral =
        {
            .register_base = M4_I2C1,
            .clock_id = PWC_FCG1_PERIPH_I2C1,
        },
};

i2c_device_config_t I2C2_config = {
    .peripheral =
        {
            .register_base = M4_I2C2,
            .clock_id = PWC_FCG1_PERIPH_I2C2,
        },
};

i2c_device_config_t I2C3_config = {
    .peripheral =
        {
            .register_base = M4_I2C3,
            .clock_id = PWC_FCG1_PERIPH_I2C3,
        },
};