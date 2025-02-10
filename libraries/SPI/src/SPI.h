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


// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

class SPISettings
{
public:
	SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
	{
		_init(clock, bitOrder, dataMode);
	}

	// Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
	SPISettings()
	{
		_init(4000000, MSBFIRST, SPI_MODE0);
	}

private:
	inline void _init(uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
	{
		this->clockFreq = clock;
		this->bitOrder = bitOrder;
		this->dataMode = dataMode;
	}

	uint32_t clockFreq;
	uint8_t dataMode;
	BitOrder bitOrder;

	friend class SPIClass;
};

class SPIClass
{
public:
	SPIClass(spi_config_t *config) 
	{
		this->config = config;
	}

	void set_mosi_pin(const gpio_pin_t pin) { this->mosi_pin = pin; }
	void set_miso_pin(const gpio_pin_t pin) { this->miso_pin = pin; }
	void set_clock_pin(const gpio_pin_t pin) { this->clock_pin = pin; }
	void set_pins(const gpio_pin_t mosi, const gpio_pin_t miso, const gpio_pin_t clock) 
	{
		this->set_mosi_pin(mosi);
		this->set_miso_pin(miso);
		this->set_clock_pin(clock);
	}

	/**
	 * @brief initialize the SPI peripheral
	 * @note you must set the MOSI, MISO and CLOCK pin before calling this function
	 */
	void begin();
	void end();

	void setClockFrequency(const uint32_t frequency);
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

	void beginTransaction(SPISettings settings);
	void endTransaction(void);

private:
	spi_config_t *config;
	gpio_pin_t mosi_pin;
	gpio_pin_t miso_pin;
	gpio_pin_t clock_pin;

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
