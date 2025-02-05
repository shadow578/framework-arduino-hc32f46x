#ifndef SOFTWARESERIAL_H
#define SOFTWARESERIAL_H

#include <Arduino.h>
#include <RingBuffer.h>
#include <Timer0.h>

#ifndef SOFTWARE_SERIAL_BUFFER_SIZE
#define SOFTWARE_SERIAL_BUFFER_SIZE 64
#endif

#ifndef SOFTWARE_SERIAL_RX_BUFFER_SIZE
#define SOFTWARE_SERIAL_RX_BUFFER_SIZE SOFTWARE_SERIAL_BUFFER_SIZE
#endif

#ifndef SOFTWARE_SERIAL_TX_BUFFER_SIZE
#define SOFTWARE_SERIAL_TX_BUFFER_SIZE SOFTWARE_SERIAL_BUFFER_SIZE
#endif

#ifndef SOFTWARE_SERIAL_OVERSAMPLE
#define SOFTWARE_SERIAL_OVERSAMPLE 3
#endif

#ifndef SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY
#define SOFTWARE_SERIAL_HALF_DUPLEX_SWITCH_DELAY 5 // bit-periods
#endif

/**
 * Software Serial implementation using Timer0.
 * loosely based on STM32duino SoftwareSerial library.
 * see https://github.com/stm32duino/Arduino_Core_STM32/blob/main/libraries/SoftwareSerial/
 */
class SoftwareSerial : public Stream
{
public:
    /**
     * @brief create a SoftwareSerial instance
     * @param rx_pin receive pin
     * @param tx_pin transmit pin
     * @param invert invert high and low on RX and TX lines
     * @note when rx_pin == tx_pin, half-duplex mode is enabled
     */
    SoftwareSerial(const gpio_pin_t rx_pin, const gpio_pin_t tx_pin, const bool invert)
    {
        this->rx_pin = rx_pin;
        this->tx_pin = tx_pin;
        this->invert = invert;
        this->is_half_duplex = rx_pin == tx_pin;
    }
    virtual ~SoftwareSerial();

    /**
     * @brief setup the software serial
     * @param baud baud rate
     * @note 
     * due to limited availability of Timers0 Units, all software serials must use the same baud rate.
     * if begin() is called with a different baud rate than the other instances, a panic is caused.
     */
    void begin(const uint32_t baud);

    /**
     * @brief de-initialize the software serial
     */
    void end();


    bool listen();
    bool isListening() { return active_listener == this; }
    bool stopListening();

    bool overflow();

    int peek();

    virtual size_t write(const uint8_t byte);
    virtual int read();
    virtual int available();
    virtual void flush();

    operator bool()
    {
        return true;
    }

    using Print::write;

private:
    const gpio_pin_t rx_pin;
    const gpio_pin_t tx_pin;
    const bool invert;
    const bool is_half_duplex;
    
    bool did_tx_overflow = false;

    RingBuffer<uint8_t> rx_buffer(SOFTWARE_SERIAL_RX_BUFFER_SIZE);
    RingBuffer<uint8_t> tx_buffer(SOFTWARE_SERIAL_TX_BUFFER_SIZE);

    void send();
    void recv();

private:
    /**
     * @brief baud rate that software serial is running at (ALL of them).
     * @note 0 if not initialized
     */
    static uint32_t initialized_baud_rate;

    /**
     * @brief Timer0 instance
     */
    static Timer0 timer;

    /**
     * @brief the software serial that is currently listening
     */
    static SoftwareSerial *active_listener;

    /**
     * @brief the software serial that is currently listening
     */
    static SoftwareSerial *active_listener;

    static volatile SoftwareSerial *active_out;
    static volatile SoftwareSerial *active_in;
}


#endif // SOFTWARESERIAL_H
