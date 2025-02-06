#include "SoftwareSerial.h"
#include <drivers/gpio/gpio.h>

static_assert(SOFTWARE_SERIAL_BUFFER_SIZE > 0, "SOFTWARE_SERIAL_BUFFER_SIZE must be > 0");
static_assert(SOFTWARE_SERIAL_OVERSAMPLE >= 3, "SOFTWARE_SERIAL_OVERSAMPLE must be >= 3");
static_assert(SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY >= 0, "SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY must be >= 0");

#ifdef __CORE_DEBUG
/*static*/ uint8_t SoftwareSerial::next_id = 0;

#define SOFTSERIAL_DEBUG_PRINTF(fmt, ...) \
    CORE_DEBUG_PRINTF("[SoftwareSerial#%u] " fmt, this->id,  ##__VA_ARGS__)

#define SOFTSERIAL_STATIC_DEBUG_PRINTF(fmt, ...) \
    CORE_DEBUG_PRINTF("[SoftwareSerial] " fmt, ##__VA_ARGS__)
#else
#define SOFTSERIAL_DEBUG_PRINTF(fmt, ...)
#define SOFTSERIAL_STATIC_DEBUG_PRINTF(fmt, ...)
#endif

SoftwareSerial::SoftwareSerial(const gpio_pin_t rx_pin, const gpio_pin_t tx_pin, const bool invert)
    : 
    #ifdef __CORE_DEBUG
    id(next_id++),
    #endif
    rx_pin(rx_pin), tx_pin(tx_pin), invert(invert)
{
    this->rx_buffer = new RingBuffer<uint8_t>(SOFTWARE_SERIAL_BUFFER_SIZE);
}

SoftwareSerial::~SoftwareSerial()
{
    end();
    delete this->rx_buffer;
}

void SoftwareSerial::begin(const uint32_t baud)
{
    SOFTSERIAL_DEBUG_PRINTF("begin: rx=%u, tx=%u, invert=%d, baud=%lu, half-duplex=%d\n", 
        rx_pin,
        tx_pin,
        invert,
        baud, 
        is_half_duplex());

    this->baud = baud;

    // half-duplex starts out in TX mode, so it is always enabled
    setup_tx();
    if (!is_half_duplex())
    {
        setup_rx();
        listen();
    }

    // make the timer ISR call this instance
    add_listener(this);
}

void SoftwareSerial::end()
{
    SOFTSERIAL_DEBUG_PRINTF("end\n");
    stopListening();
    remove_listener(this);
}

bool SoftwareSerial::listen()
{
    rx_wait_ticks = 1; // next interrupt will check for start bit
    rx_bit_count = -1; // wait for start bit

    // change the speed of the timer
    // this function automatically waits for all pending TX operations to finish
    const bool did_speed_change = timer_set_speed(baud);

    // enable RX
    if (is_half_duplex())
    {
        set_half_duplex_mode(true /*=RX*/);
    }
    else
    {
        enable_rx = true;
    }

    SOFTSERIAL_DEBUG_PRINTF("started listening @baud=%lu; did_speed_change=%d\n", baud, did_speed_change);
    return did_speed_change;
}

bool SoftwareSerial::isListening()
{
    return current_timer_speed == baud;
}

bool SoftwareSerial::stopListening()
{
    // wait for any pending TX operations to finish
    while (enable_tx)
        yield();
    
    // disable RX
    const bool was_listening = enable_rx || enable_tx;
    if (is_half_duplex())
    {
        set_half_duplex_mode(false /*=TX*/);
    }
    else
    {
        enable_rx = false;
    }

    // if no other instance is listening, stop the timer
    bool any_listening = false;
    ListenerItem *item = listeners;
    while (item != nullptr)
    {
        if (item->listener->isListening())
        {
            any_listening = true;
            break;
        }

        item = item->next;
    }

    if (!any_listening)
    {
        timer_set_speed(0);
    }

    SOFTSERIAL_DEBUG_PRINTF("stopped listening; was_listening=%d\n", was_listening);
    return was_listening;
}

bool SoftwareSerial::overflow()
{
    const bool overflow = did_rx_overflow;
    did_rx_overflow = false;
    return overflow;
}

int SoftwareSerial::peek()
{
    return rx_buffer->peek();
}

size_t SoftwareSerial::write(const uint8_t byte)
{
    // wait for previous TX to finish
    tx_pending = true;
    while (enable_tx)
        yield();

    // add start and stop bits
    tx_frame = (byte << 1) | 0x200;
    if (invert)
    {
        tx_frame = ~tx_frame;
    }

    // ensure timer is running at the correct speed
    timer_set_speed(baud);

    // ensure TX is enabled in half-duplex mode
    // this call is a no-op if not in half-duplex mode, so no additional check
    set_half_duplex_mode(false /*=TX*/);

    // start transmission on next interrupt
    tx_bit_count = 0;
    tx_wait_ticks = 1;

    tx_pending = false;
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

void SoftwareSerial::setup_rx()
{
    SOFTSERIAL_DEBUG_PRINTF("setup_rx on %u\n", rx_pin);
    pinMode(rx_pin, invert ? INPUT : INPUT_PULLUP);
}

void SoftwareSerial::setup_tx()
{
    SOFTSERIAL_DEBUG_PRINTF("setup_tx on %u\n", tx_pin);

    // set pin level before setting as output to avoid glitches
    if (invert) GPIO_ResetBits(tx_pin);
    else GPIO_SetBits(tx_pin);

    pinMode(tx_pin, OUTPUT);
}

void SoftwareSerial::set_half_duplex_mode(const bool rx)
{
    // if not half-duplex mode, ignore this
    if (!is_half_duplex()) return;

    if (rx)
    {
        enable_tx = false;
        setup_rx();

        rx_bit_count = -1; // waiting for start bit
        rx_wait_ticks = 2; // wait 2 bit times for start bit
        enable_rx = true;
    }
    else
    {
        if (enable_rx)
        {
            enable_rx = false;
            setup_tx();
        }
    }
}

//
// ISR
//

void SoftwareSerial::do_rx()
{
    // if not enabled, do nothing
    if (!enable_rx) return;

    // if tick count is non-zero, continue waiting
    rx_wait_ticks--;
    if (rx_wait_ticks > 0) return;

    // read bit, invert if inverted logic
    const bool bit = GPIO_GetBit(rx_pin) ^ invert;

    // waiting for start bit?
    if (rx_bit_count == -1)
    {
        // TODO: is this correct?? i though start bit was going HIGH, but this is how STM32 does it...
        if (!bit)
        {
            // got start bit
            rx_frame = 0;
            rx_bit_count = 0;

            // wait 1 1/2 bit times to sample in the middle of the bit
            rx_wait_ticks = SOFTWARE_SERIAL_OVERSAMPLE + (SOFTWARE_SERIAL_OVERSAMPLE >> 1);
        }
        else
        {
            // waiting for start bit, but didn't get it
            // wait for next interrupt to check again
            rx_wait_ticks = 1;
        }
    }
    else if (rx_bit_count >= 8) // waiting for stop bit?
    {
        // TODO: is this correct?? i though stop bit was going LOW, but this is how STM32 does it...
        if (bit)
        {
            // got stop bit, add byte to buffer
            bool overflow;
            rx_buffer->push(rx_frame, true, overflow);

            // avoid overwriting overflow flag
            if (overflow) did_rx_overflow = true;
        }

        // assume frame is completed, wait for next start bit at next interrupt
        rx_bit_count = -1;
        rx_wait_ticks = 1;
    }
    else // data bits
    {
        rx_frame >>= 1;
        if (bit) rx_frame |= 0x80;
        rx_bit_count++;
        rx_wait_ticks = SOFTWARE_SERIAL_OVERSAMPLE;
    }

}

void SoftwareSerial::do_tx()
{
    // if not enabled, do nothing
    if (!enable_tx) return;

    // if tick count is non-zero, continue waiting
    tx_wait_ticks--;
    if (tx_wait_ticks > 0) return;

    // all bits in frame sent?
    if (tx_bit_count >= 10)
    {
        // if no frame is pending and half-duplex, switch to RX mode
        // otherwise, we're done transmitting
        if (!tx_pending && is_half_duplex())
        {
            // wait HALF_DUPLEX_SWITCH_DELAY bits before switching to RX
            if (tx_bit_count >= 10 + SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY)
            {
                set_half_duplex_mode(true /*=RX*/);
            }
        }
        else
        {
            enable_tx = false;
        }
    }

    // send next bit
    if (tx_frame & 1) GPIO_SetBits(tx_pin); // mark
    else GPIO_ResetBits(tx_pin); // space

    tx_frame >>= 1;
    tx_bit_count++;
    tx_wait_ticks = SOFTWARE_SERIAL_OVERSAMPLE;
}

//
// Timer control
//

/*static*/ uint32_t SoftwareSerial::current_timer_speed = 0;
/*static*/ Timer0 SoftwareSerial::timer(&SOFTWARE_SERIAL_TIMER0_UNIT, SoftwareSerial::timer_isr);
/*static*/ SoftwareSerial::ListenerItem *SoftwareSerial::listeners = nullptr;

/*static*/ bool SoftwareSerial::timer_set_speed(const uint32_t baud)
{
    if (current_timer_speed == baud) return false;

    // speed change operations are fairly costly because they block until pending TX operations finish
    // so print a warning if this happens
    if (current_timer_speed != 0)
    {
        SOFTSERIAL_STATIC_DEBUG_PRINTF("baud rate change from %lu to %lu. Consider configuring all your software serials to the same baud rate to improve performance.\n", 
            current_timer_speed, 
            baud);

        // wait for all pending TX operations in active channels to finish before changing speed
        ListenerItem *item = listeners;
        while (item != nullptr)
        {
            while (item->listener->enable_tx)
                yield();

            item = item->next;
        }
    }

    SOFTSERIAL_STATIC_DEBUG_PRINTF("timer_set_speed baud=%lu\n", baud);

    // (re-) initialize timer to the baud rate frequency, with oversampling
    // if already running, timer will automatically stop in the start() call
    timer.start(baud * SOFTWARE_SERIAL_OVERSAMPLE, SOFTWARE_SERIAL_TIMER_PRESCALER);
    current_timer_speed = baud;
    timer.resume(); // needed to actually start the timer
    return true;

} 

/*static*/ void SoftwareSerial::add_listener(SoftwareSerial *listener)
{
    // pause timer while modifying listener list to avoid race conditions
    timer.pause();

    ListenerItem *item = new ListenerItem;
    item->listener = listener;
    item->next = listeners;
    listeners = item;

    timer.resume();
}

/*static*/ void SoftwareSerial::remove_listener(SoftwareSerial *listener)
{
    ListenerItem *prev = nullptr;
    ListenerItem *item = listeners;
    while (item != nullptr)
    {
        if (item->listener == listener)
        {
            // pause timer while modifying listener list to avoid race conditions
            timer.pause();

            if (prev == nullptr)
            {
                listeners = item->next;
            }
            else
            {
                prev->next = item->next;
            }

            timer.resume();

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
        // only call RX/TX if instance uses the correct baud rate
        if (item->listener->isListening())
        {
            item->listener->do_tx();
            item->listener->do_rx();
        }
        
        item = item->next;
    }
}
