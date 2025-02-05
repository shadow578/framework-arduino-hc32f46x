#include "SoftwareSerial.h"

uint32_t SoftwareSerial::initialized_baud_rate = 0;
Timer0 SoftwareSerial::timer;
SoftwareSerial *SoftwareSerial::active_listener = nullptr;
SoftwareSerial *SoftwareSerial::active_out = nullptr;
SoftwareSerial *SoftwareSerial::active_in = nullptr;

void SoftwareSerial::begin(const uint32_t baud)
{
    // already initialized to a different baud rate?
    if (initialized_baud_rate != 0 && initialized_baud_rate != baud)
    {
        panic("all SoftwareSerials must use the same baud rate!");
        return;
    }

    set_tx();
    if (!is_half_duplex)
    {
        set_rx();
        listen();
    }
}

void SoftwareSerial::end()
{
    stopListening();
}

bool SoftwareSerial::listen()
{

}

bool SoftwareSerial::stopListening()
{

}

bool SoftwareSerial::overflow()
{
    if (did_tx_overflow)
    {
        did_tx_overflow = false;
        return true;
    }

    return false;
}

int SoftwareSerial::peek()
{
    return rx_buffer.peek();
}

size_t SoftwareSerial::write(const uint8_t byte)
{

}

int SoftwareSerial::read()
{
    uint8_t e;
    if (rx_buffer.pop(e))
    {
        return e;
    }

    return -1;
}

int SoftwareSerial::available()
{
    return rx_buffer.count();
}

void SoftwareSerial::flush()
{
    rx_buffer.clear();
}
