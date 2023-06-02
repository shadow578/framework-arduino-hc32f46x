#pragma once
#include "Arduino.h"
#include <hc32_ddl.h>
#include "timer0_config.h"

/**
 * @brief timer0 base frequency
 * @note timer0 uses PCLK1 as base frequency. with custom config, this can be changed
 */
#define TIMER0_BASE_FREQUENCY (SYSTEM_CLOCK_FREQUENCIES.pclk1)

/**
 * @brief timer0 channel enum class
 */
enum class Timer0Channel
{
    /**
     * @brief timer0 unit channel A
     */
    A,

    /**
     * @brief timer0 unit channel B
     */
    B,
};

class Timer0
{
public:
    /**
     * @brief Construct a new Timer0 object
     * @param config pointer to timer0 peripheral configuration
     * @note Timer0 Unit 1 and 2 are not identical. Unit 1 only supports async mode, while Unit 2 supports both.
     */
    Timer0(timer0_config_t *config);

    /**
     * @brief start timer0 channel with frequency and prescaler
     * @param channel timer0 channel to start
     * @param frequency the frequency to set the timer to
     * @param prescaler the prescaler to use. must be one of [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
     * @note compare value = (pclk1 / prescaler) / frequency
     * @note this function will not automatically start the timer interrupt. call resume() to start the interrupt
     * @note only supports Timer0 Unit 2 (M4_TMR02)
     */
    void start(const Timer0Channel channel, const uint32_t frequency, const uint16_t prescaler = 1);

    /**
     * @brief start timer0 channel with custom channel config
     * @param channel timer0 channel to start
     * @param channel_config pointer to timer0 channel configuration
     * @note this function will not automatically start the timer interrupt. call resume() to start the interrupt
     */
    void start(const Timer0Channel channel, const stc_tim0_base_init_t *channel_config);

    /**
     * @brief stop timer0 channel
     * @param channel timer0 channel to stop
     * @note this function will automatically stop the timer interrupt
     */
    void stop(const Timer0Channel channel);

    /**
     * @brief pause timer0 channel interrupt
     * @param channel timer0 channel to pause
     * @note if the channel is not start()-ed, this function will do nothing
     */
    void pause(const Timer0Channel channel);

    /**
     * @brief resume timer0 channel interrupt
     * @param channel timer0 channel to resume
     * @note if the channel is not start()-ed, this function will do nothing
     */
    void resume(const Timer0Channel channel);

    /**
     * @brief check if timer0 channel interrupt is currently paused
     * @param channel timer0 channel to check
     * @return true if paused
     * @note if the channel is not start()-ed, this function will return false
     */
    bool isPaused(const Timer0Channel channel);

    /**
     * @brief set timer0 channel compare value
     * @param channel timer0 channel to set compare value
     * @param compare compare value to set
     * @note the channel must be start()-ed before calling this function
     */
    void setCompareValue(const Timer0Channel channel, const uint16_t compare);

    /**
     * @brief get timer0 channel counter value
     * @param channel timer0 channel to get counter value
     * @return current counter value
     * @note the channel must be start()-ed before calling this function
     */
    uint16_t getCount(const Timer0Channel channel);

    /**
     * @brief set timer0 channel callback
     * @param channel timer0 channel to set callback for
     * @param callback callback function to set. NULL to remove callback
     */
    void setCallback(const Timer0Channel channel, voidFuncPtr callback);

    /**
     * @brief set timer0 channel callback priority
     * @param channel timer0 channel to set callback priority for
     * @param priority priority to set
     */
    void setCallbackPriority(const Timer0Channel channel, const uint32_t priority);

    /**
     * @brief remove timer0 channel callback
     * @param channel timer0 channel to remove callback for
     */
    void removeCallback(const Timer0Channel channel)
    {
        setCallback(channel, NULL);
    }

private:
    timer0_config_t *config;

    bool channel_a_initialized = false;
    bool channel_b_initialized = false;

    bool isChannelInitialized(const Timer0Channel channel)
    {
        switch (channel)
        {
        case Timer0Channel::A:
            return channel_a_initialized;
        case Timer0Channel::B:
            return channel_b_initialized;
        default:
            return false;
        }
    }

    void setChannelInitialized(const Timer0Channel channel, bool initialized)
    {
        switch (channel)
        {
        case Timer0Channel::A:
            channel_a_initialized = initialized;
            break;
        case Timer0Channel::B:
            channel_b_initialized = initialized;
            break;
        default:
            break;
        }
    }
};
