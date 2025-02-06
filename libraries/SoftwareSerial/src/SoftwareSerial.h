#ifndef SOFTWARESERIAL_H
#define SOFTWARESERIAL_H

#include <Arduino.h>
#include <RingBuffer.h>
#include <Timer0.h>

#ifndef SOFTWARE_SERIAL_BUFFER_SIZE
#define SOFTWARE_SERIAL_BUFFER_SIZE 64
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
    SoftwareSerial(const gpio_pin_t rx_pin, const gpio_pin_t tx_pin, const bool invert = false)
        : rx_pin(rx_pin), tx_pin(tx_pin), invert(invert)
    {
        this->rx_buffer = new RingBuffer<uint8_t>(SOFTWARE_SERIAL_BUFFER_SIZE);
    }
    ~SoftwareSerial()
    {
        delete this->rx_buffer;
    }

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


    bool listen() { return false; }
    bool isListening() { return true; }
    bool stopListening() { return true; }

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

    inline bool is_half_duplex()
    {
        return rx_pin == tx_pin;
    }

private: // RX logic
    RingBuffer<uint8_t> *rx_buffer;
    bool did_rx_overflow : 1;
    bool enable_rx : 1;

    uint16_t current_rx_frame = 0;
    uint8_t rx_frame_bits_count = 0xff; // 0xff = waiting for start bit

    /**
     * @brief receive a single bit. called by the timer ISR
     */
    void do_rx();

private: // TX logic
    bool enable_tx : 1;
    bool tx_pending : 1;

    uint16_t current_tx_frame = 0; // 10 bits
    uint8_t tx_frame_bits_count = 0;
    int8_t tx_tick_count = 0;

    /**
     * @brief transmit a single bit. called by the timer ISR
     */
    void do_tx();

private: // Timer0 ISR logic
    /**
     * @brief baud rate that software serial is running at (ALL of them).
     * @note 0 if not initialized
     */
    static uint32_t initialized_baud_rate;

    /**
     * @brief Timer0 instance
     */
    static Timer0 timer;

    struct ListenerItem
    {
        SoftwareSerial *listener;
        ListenerItem *next;
    };

    /**
     * @brief list of software serials that should be called in the timer ISR 
     */
    static ListenerItem *listeners;

    static void add_listener(SoftwareSerial *listener);
    static void remove_listener(SoftwareSerial *listener);

    /**
     * @brief timer callback
     */
    static void timer_isr();
};

#endif // SOFTWARESERIAL_H
