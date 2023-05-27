/*
 Copyright (c) 2015 Arduino LLC.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SPI_H_
#define SPI_H_

#error "SPI library is not yet supported on the HC32F460"

#include "Arduino.h"

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
		init_AlwaysInline(clock, bitOrder, dataMode);
	}

	// Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
	SPISettings()
	{
		init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
	}

private:
	void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) __attribute__((__always_inline__))
	{
		this->clockFreq = clock;
		this->bitOrder = bitOrder;
		this->dataMode = dataMode;
	}

	uint32_t clockFreq;
	uint8_t dataMode;
	uint32_t bitOrder;

	friend class SPIClass;
};

class SPIClass
{
public:
	// Parameters passed to the constructor are typically used for assigning the pins etc.
	SPIClass();

	byte transfer(uint8_t data);
	uint16_t transfer16(uint16_t data);
	inline void transfer(void *buf, size_t count);

	// Transaction Functions
	void usingInterrupt(int interruptNumber);
	void beginTransaction(SPISettings settings);
	void endTransaction(void);

	// SPI Configuration methods
	void attachInterrupt();
	void detachInterrupt();

	void begin();
	void end();

	void setBitOrder(BitOrder order);
	void setDataMode(unsigned char mode);
	void setClockDivider(unsigned char div);
};

void SPIClass::transfer(void *buf, size_t count)
{
	uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
	for (size_t i = 0; i < count; i++)
	{
		*buffer = transfer(*buffer);
		buffer++;
	}
}

extern SPIClass SPI;

#endif /* SPI_H_ */
