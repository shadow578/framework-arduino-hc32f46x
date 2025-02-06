#ifndef SOFTWARESERIAL_H
#define SOFTWARESERIAL_H

#warning "SoftwareSerial on HC32F460 is experimental!"

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

#ifndef SOFTWARE_SERIAL_TIMER_PRESCALER
#define SOFTWARE_SERIAL_TIMER_PRESCALER 2
#endif

#ifndef SOFTWARE_SERIAL_TIMER0_UNIT
// recommented to not use TIMER0 Unit 1 Channel A, as it does not support sync mode
#define SOFTWARE_SERIAL_TIMER0_UNIT TIMER01B_config // Timer0 Unit 1, Channel B
#endif

/**
 * Software Serial implementation using Timer0.
 * loosely based on STM32duino SoftwareSerial library.
 * see https://github.com/stm32duino/Arduino_Core_STM32/blob/main/libraries/SoftwareSerial/
 * 
 * @note
 * This SoftwareSerial implementation has some caveats due to technical limitations:
 * a) While you may define as many software serial instances as you want, there can 
 *    only ever be one active baud rate at a time.
 *    This means that two instances at the same baud rate can run at the same time,
 *    but if you write to a software serial instance with a different baud rate,
 *    the other instances will stop sending and receiving data until you call listen() on one of them.
 * b) Switching the baud rate is fairly slow, because it waits for all pending TX operations to finish.
 *    Additionally, a baud rate switch may cause data loss on the RX side.
 *    Due to this, it is recommended to configure all software serial instances to the same baud rate.
 * c) The timer prescaler must be manually chosen to match the desired baud rate range.
 *    Set SOFTWARE_SERIAL_TIMER_PRESCALER such that the frequency error is minimal.
 *    The default value of 2 is good for PCLK1=50MHz and OVERSAMPLE=3 for baud rates 
 *    between1200 to 38400 baud, with < 0.01% error.
 *    For baud rates between 38400 and 115200, a prescaler of 1 is recommended (< 0.5% error).
 */
class SoftwareSerial : public Stream
{
#ifdef __CORE_DEBUG
private:
    static uint8_t next_id;
    const uint8_t id;
#endif

public:
    /**
     * @brief create a SoftwareSerial instance
     * @param rx_pin receive pin
     * @param tx_pin transmit pin
     * @param invert invert high and low on RX and TX lines
     * @note when rx_pin == tx_pin, half-duplex mode is enabled
     */
    SoftwareSerial(const gpio_pin_t rx_pin, const gpio_pin_t tx_pin, const bool invert = false);
    virtual ~SoftwareSerial();

    /**
     * @brief setup the software serial
     * @param baud baud rate
     */
    void begin(const uint32_t baud);

    /**
     * @brief de-initialize the software serial
     */
    void end();

    /**
     * @brief start listening for incoming data
     * @returns true if a speed change occurred. 
     * If this is the case, serials using a different baud rate will 
     * stop sending and receiving data util listen() is called on them. 
     */
    bool listen();

    /**
     * @brief check if this software serial is listening
     * @note 
     * multiple software serials can be listening at the same time, 
     * as long as they are using the same baud rate.
     */
    bool isListening();

    /**
     * @brief stop listening for incoming data
     * @returns true if this software serial was previously listening
     */
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

private: // common
    const gpio_pin_t rx_pin;
    const gpio_pin_t tx_pin;
    const bool invert;
    uint32_t baud;

    inline bool is_half_duplex()
    {
        return rx_pin == tx_pin;
    }

    /**
     * @brief setup RX pin for receiving
     */
    void setup_rx();

    /**
     * @brief setup TX pin for transmitting
     */
    void setup_tx();

    /**
     * @brief setup RX and TX pins for half-duplex communication
     * @param rx true for RX, false for TX
     * @note no-op if not in half-duplex mode
     */
    void set_half_duplex_mode(const bool rx);

private: // RX logic
    RingBuffer<uint8_t> *rx_buffer;
    bool did_rx_overflow : 1;
    bool enable_rx : 1;

    uint8_t rx_frame = 0; // 8 bits
    int8_t rx_bit_count = -1; // -1 means waiting for start bit
    int8_t rx_wait_ticks = 0;

    /**
     * @brief receive a single bit. called by the timer ISR
     */
    void do_rx();

private: // TX logic
    bool enable_tx : 1;
    bool tx_pending : 1;

    uint16_t tx_frame = 0; // 10 bits
    int8_t tx_bit_count = 0;
    int8_t tx_wait_ticks = 0;

    /**
     * @brief transmit a single bit. called by the timer ISR
     */
    void do_tx();

private: // Timer0 ISR logic
    /**
     * @brief baud rate that software serial is running at (ALL of them).
     * @note 0 if not initialized
     */
    static uint32_t current_timer_speed;

    /**
     * @brief Timer0 instance
     */
    static Timer0 timer;

    /**
     * @brief set the timer baud rate
     * @param baud baud rate to set the timer to. 0 to stop the timer
     * @return true if a speed change occurred
     */
    static bool timer_set_speed(const uint32_t baud);

    struct ListenerItem
    {
        SoftwareSerial *listener;
        ListenerItem *next;
    };

    /**
     * @brief list of software serials that should be called in the timer ISR 
     */
    static ListenerItem *listeners;

    /**
     * @brief add a listener to the timer ISR
     * @param listener software serial instance to add
     */
    static void add_listener(SoftwareSerial *listener);

    /**
     * @brief remove a listener from the timer ISR
     * @param listener software serial instance to remove
     */
    static void remove_listener(SoftwareSerial *listener);

    /**
     * @brief timer callback
     */
    static void timer_isr();
};

#endif // SOFTWARESERIAL_H
