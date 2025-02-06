#include "SoftwareSerial.h"
#include <drivers/gpio/gpio.h>

uint32_t SoftwareSerial::initialized_baud_rate = 0;
Timer0 SoftwareSerial::timer(&TIMER01B_config, SoftwareSerial::timer_isr); // TODO allow changing timer unit
SoftwareSerial::ListenerItem *SoftwareSerial::listeners = nullptr;

void SoftwareSerial::begin(const uint32_t baud)
{
    // already initialized to a different baud rate?
    if (initialized_baud_rate != 0 && initialized_baud_rate != baud)
    {
        panic("all SoftwareSerials must use the same baud rate!");
        return;
    }
    initialized_baud_rate = baud;

    // setup timer
    timer.start(baud * SOFTWARE_SERIAL_OVERSAMPLE);

    //set_tx();
    //if (!is_half_duplex)
    //{
    //    set_rx();
    //    listen();
    //}

    add_listener(this);
}

void SoftwareSerial::end()
{
    stopListening();
    remove_listener(this);
}

bool SoftwareSerial::overflow()
{
    if (did_rx_overflow)
    {
        did_rx_overflow = false;
        return true;
    }

    return false;
}

int SoftwareSerial::peek()
{
    return rx_buffer->peek();
}

size_t SoftwareSerial::write(const uint8_t byte)
{
    // wait for previous TX to finish
    while (enable_tx)
        yield();

    // add start and stop bits
    current_tx_frame = (byte << 1) | 0x200;
    if (invert)
    {
        current_tx_frame = ~current_tx_frame;
    }

    // start transmission
    tx_frame_bits_count = 0;
    tx_tick_count = 0;
    enable_tx = true;
    return 1;
}

int SoftwareSerial::read()
{
    uint8_t e;
    if (rx_buffer->pop(e))
    {
        return e;
    }

    return -1;
}

int SoftwareSerial::available()
{
    return rx_buffer->count();
}

void SoftwareSerial::flush()
{
    // TODO implement how
}

//
// ISR
//

void SoftwareSerial::do_rx()
{
    // if not enabled, do nothing
    if (!enable_rx) return;

    // if tick count is non-zero, continue waiting
    rx_tick_count--;
    if (rx_tick_count > 0) return;

    const bool bit = GPIO_GetBit(rx_pin) ^ invert;

    // waiting for start bit?
    if (rx_frame_bits_count == -1)
    {
        // TODO: is this correct??
        if (!bit)
        {
            // got start bit
            current_rx_frame = 0;
            rx_frame_bits_count = 0;

            // wait 1 1/2 bit times to sample in the middle of the bit
            rx_tick_count = SOFTWARE_SERIAL_OVERSAMPLE + (SOFTWARE_SERIAL_OVERSAMPLE >> 1);
        }
        else
        {
            // waiting for start bit, but didn't get it
            // wait for next interrupt to check again
            rx_tick_count = 1;
        }
    }
    else if (rx_frame_bits_count >= 8) // waiting for stop bit?
    {
        if (bit)
        {
            // got stop bit, add byte to buffer
            bool overflow;
            rx_buffer->push(current_rx_frame, true, overflow);

            // avoid overwriting overflow flag
            if (overflow) did_rx_overflow = true;
        }

        // assume frame is completed, wait for next start bit at next interrupt
        rx_frame_bits_count = -1;
        rx_tick_count = 1;
    }
    else // data bits
    {
        current_rx_frame >>= 1;
        if (bit) current_rx_frame |= 0x80;
        rx_frame_bits_count++;
        rx_tick_count = SOFTWARE_SERIAL_OVERSAMPLE;
    }

}

void SoftwareSerial::do_tx()
{
    // if not enabled, do nothing
    if (!enable_tx) return;

    // if tick count is non-zero, continue waiting
    tx_tick_count--;
    if (tx_tick_count > 0) return;

    // all bits in frame sent?
    if (tx_frame_bits_count >= 10)
    {
        // if no frame is pending and half-duplex, switch to RX mode
        // otherwise, we're done transmitting
        if (!tx_pending && is_half_duplex())
        {
            // wait HALF_DUPLEX_SWITCH_DELAY bits before switching to RX
            if (tx_frame_bits_count >= 10 + SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY)
            {
                // TODO start RX
            }
        }
        else
        {
            enable_tx = false;
        }
    }

    // send next bit
    if (current_tx_frame & 1) GPIO_SetBits(tx_pin); // mark
    else GPIO_ResetBits(tx_pin); // space

    current_tx_frame >>= 1;
    tx_frame_bits_count++;
    tx_tick_count = SOFTWARE_SERIAL_OVERSAMPLE;
}

/*static*/ void SoftwareSerial::add_listener(SoftwareSerial *listener)
{
    ListenerItem *item = new ListenerItem;
    item->listener = listener;
    item->next = listeners;
    listeners = item;
}

/*static*/ void SoftwareSerial::remove_listener(SoftwareSerial *listener)
{
    ListenerItem *prev = nullptr;
    ListenerItem *item = listeners;
    while (item != nullptr)
    {
        if (item->listener == listener)
        {
            if (prev == nullptr)
            {
                listeners = item->next;
            }
            else
            {
                prev->next = item->next;
            }

            delete item;
            return;
        }

        prev = item;
        item = item->next;
    }
}

/*static*/ void SoftwareSerial::timer_isr()
{
    ListenerItem *item = listeners;
    while (item != nullptr)
    {
        item->listener->do_tx();
        item->listener->do_rx();
        
        item = item->next;
    }
}
