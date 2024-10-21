#ifndef SPI_H_
#define SPI_H_

#include "Arduino.h"
#include <hc32_ddl.h>
#include "spi_config.h"

// check DDL configuration
#if (DDL_SPI_ENABLE != DDL_ON)
#error "SPI library requires SPI DDL to be enabled"
#endif

#if (DDL_PWC_ENABLE != DDL_ON)
#error "SPI library requires PWC DDL to be enabled"
#endif

class SPIClass
{
public:
	SPIClass(spi_config_t *config) 
	{
		this->config = config;
	}

	void begin(const gpio_pin_t mosi_pin, const gpio_pin_t miso_pin, const gpio_pin_t clock_pin);
	void end();

	void setClockDivider(const uint16_t divider);
	void setBitOrder(const BitOrder order);

	inline uint8_t transfer(const uint8_t data)
	{
		send(SpiDataLengthBit8, static_cast<uint32_t>(data));
		return static_cast<uint8_t>(receive());
	}

	inline uint16_t transfer16(const uint16_t data)
	{
		send(SpiDataLengthBit16, static_cast<uint32_t>(data));
		return static_cast<uint16_t>(receive());
	}

	inline uint32_t transfer32(const uint32_t data)
	{
		send(SpiDataLengthBit32, data);
		return receive();
	}

	inline void transfer(uint8_t *buffer, const size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			*buffer = transfer(*buffer);
			buffer++;
		}
	}

private:
	spi_config_t *config;

	/**
	 * @brief synchronously send data_len bits of data
	 * @param data_len number of bits to send. reconfigures the SPI peripheral to this data length before sending.
	 * @param data data to send
	 */
	void send(const en_spi_data_length_t data_len, const uint32_t data);
	
	/**
	 * @brief synchronously receive data
	 * @return received data
	 * @note data lenght must be set before to-be received data is sent by the other side, e.g. by using send()
	 */
	uint32_t receive();
};

/**
 * @brief global SPI instance, uses SPI1 peripheral.
 * @note 3 other SPI peripherals are available (SPI2-4), but need to be instanciated manually
 */
extern SPIClass SPI1;

// alias SPI1 to SPI for compatibility with libraries
#define SPI SPI1

#endif /* SPI_H_ */
