#include "spi_config.h"

spi_config_t SPI1_config = {
    .register_base = M4_SPI1,
    .clock_id = PWC_FCG1_PERIPH_SPI1,
    .mosi_func = Func_Spi3_Mosi,
    .miso_func = Func_Spi3_Miso,
    .sck_func = Func_Spi3_Sck,
};

spi_config_t SPI2_config = {
    .register_base = M4_SPI2,
    .clock_id = PWC_FCG1_PERIPH_SPI2,
    .mosi_func = Func_Spi2_Mosi,
    .miso_func = Func_Spi2_Miso,
    .sck_func = Func_Spi2_Sck,
};

spi_config_t SPI3_config = {
    .register_base = M4_SPI3,
    .clock_id = PWC_FCG1_PERIPH_SPI3,
    .mosi_func = Func_Spi3_Mosi,
    .miso_func = Func_Spi3_Miso,
    .sck_func = Func_Spi3_Sck,
};

spi_config_t SPI4_config = {
    .register_base = M4_SPI4,
    .clock_id = PWC_FCG1_PERIPH_SPI4,
    .mosi_func = Func_Spi4_Mosi,
    .miso_func = Func_Spi4_Miso,
    .sck_func = Func_Spi4_Sck,
};
