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

#ifndef TWO_WIRE_H_
#define TWO_WIRE_H_

#error "Wire library is not yet supported on the HC32F460"

#include "Stream.h"
#include "variant.h"
#include "RingBuffer.h"

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire: public Stream {
public:
	TwoWire();

	void begin();
	void begin(uint8_t address);

	void end();
	void setClock(uint32_t clockFreq);

	void beginTransmission(uint8_t address);
	uint8_t endTransmission(bool stopBit = true);

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit = true);

	size_t write(uint8_t data);
	size_t write(const uint8_t * data, size_t quantity);

	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);

	using Print::write;

private:
    RingBuffer rxBuffer;
    RingBuffer txBuffer;
};

extern TwoWire Wire;

#endif /* TWO_WIRE_H_ */
