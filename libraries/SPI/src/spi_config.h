#pragma once
#include <hc32_ddl.h>

/**
 * @brief SPI peripheral configuration
 */
typedef struct spi_config_t
{
    /**
     * @brief The base address of the SPI peripheral.
     */
    M4_SPI_TypeDef *register_base;
    
    /**
     * @brief SPI peripheral channel.
     */
    uint32_t clock_id;

    /**
     * @brief SPI peripheral MOSI pin function
     */
    en_port_func_t mosi_func;

    /**
     * @brief SPI peripheral MISO pin function
     */
    en_port_func_t miso_func;

    /**
     * @brief SPI peripheral SCK pin function
     */
    en_port_func_t sck_func;

} spi_config_t;

/**
 * @brief SPI1 configuration
 */
extern spi_config_t SPI1_config;

/**
 * @brief SPI2 configuration
 */
extern spi_config_t SPI2_config;

/**
 * @brief SPI3 configuration
 */
extern spi_config_t SPI3_config;

/**
 * @brief SPI4 configuration
 */
extern spi_config_t SPI4_config;
