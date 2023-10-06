#pragma once

// check DDL configuration
#if (DDL_TIMER0_ENABLE != DDL_ON)
#error "Timer0 library requires TIMER0 DDL to be enabled"
#endif

#if (DDL_PWC_ENABLE != DDL_ON)
#error "Timer0 library requires PWC DDL to be enabled"
#endif

#include "Arduino.h"
#include <core_debug.h>
#include <hc32_ddl.h>
#include "timer0_config.h"

/**
 * @brief timer0 base frequency
 * @note timer0 uses PCLK1 as base frequency. with custom config, this can be changed
 */
#define TIMER0_BASE_FREQUENCY (SYSTEM_CLOCK_FREQUENCIES.pclk1)

/**
 * @brief timer0 lib inline function attribute
 */
#define TIMER0_INLINE_ATTR __attribute__((always_inline)) inline

class Timer0
{
public:
    /**
     * @brief Construct a new Timer0 object
     * @param config pointer to timer0 peripheral configuration
     * @param callback pointer to callback function for timer interrupt
     */
    Timer0(timer0_channel_config_t *config, const voidFuncPtr callback);

    /**
     * @brief start timer0 channel with frequency and prescaler
     * @param frequency the frequency to set the timer to
     * @param prescaler the prescaler to use. must be one of [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
     * @note compare value = (base_freq / prescaler) / frequency
     * @note Timer0 Unit 1 Channel A will use LRC clock (32KHz) instead of PCLK1 (EXPERIMENTAL, might not work properly)
     * @note this function will not automatically start the timer interrupt. call resume() to start the interrupt
     * @note if the channel is already start()-ed, this function will stop the channel first
     */
    void start(const uint32_t frequency, const uint16_t prescaler = 1);

    /**
     * @brief start timer0 channel with custom channel config
     * @param channel_config pointer to timer0 channel start configuration
     * @note this function will not automatically start the timer interrupt. call resume() to start the interrupt
     * @note if the channel is already start()-ed, this function will stop the channel first
     */
    void start(const stc_tim0_base_init_t *channel_config);

    /**
     * @brief stop timer0 channel
     * @note this function will automatically stop the timer interrupt
     * @note if the channel is not start()-ed, this function will do nothing
     * @note if all channels are stopped, the timer0 peripheral will be disabled automatically
     */
    void stop();

    //
    // inlined functions
    // (inlined since they are somewhat timing-critical)
    //

    /**
     * @brief pause timer0 channel interrupt
     * @param channel timer0 channel to pause
     * @note if the channel is not start()-ed, this function will do nothing
     */
    TIMER0_INLINE_ATTR void pause()
    {
        // if not initialized, return false
        if (!this->isStarted)
        {
            return;
        }

        TIMER0_Cmd(this->config->peripheral.register_base, this->config->peripheral.channel, Disable);
    }

    /**
     * @brief resume timer0 channel interrupt
     * @param channel timer0 channel to resume
     * @note if the channel is not start()-ed, this function will do nothing
     */
    TIMER0_INLINE_ATTR void resume()
    {
        // if not initialized, return false
        if (!this->isStarted)
        {
            return;
        }

        TIMER0_Cmd(this->config->peripheral.register_base, this->config->peripheral.channel, Enable);
    }

    /**
     * @brief check if timer0 channel interrupt is currently paused
     * @param channel timer0 channel to check
     * @return true if paused
     * @note if the channel is not start()-ed, this function will return false
     */
    TIMER0_INLINE_ATTR bool isPaused()
    {
        // if not initialized, return false
        if (!this->isStarted)
        {
            return false;
        }

        // otherwise, return the channel status
        if (this->config->peripheral.channel == Tim0_ChannelA)
        {
            return !bool(this->config->peripheral.register_base->BCONR_f.CSTA);
        }
        else
        {
            return !bool(this->config->peripheral.register_base->BCONR_f.CSTB);
        }
    }

    /**
     * @brief set timer0 channel compare value
     * @param channel timer0 channel to set compare value
     * @param compare compare value to set
     * @note the channel must be start()-ed before calling this function
     */
    TIMER0_INLINE_ATTR void setCompareValue(const uint16_t compare)
    {
        //CORE_ASSERT(this->isStarted, "Timer0::setCompare(): channel not initialized");
        TIMER0_WriteCmpReg(this->config->peripheral.register_base, this->config->peripheral.channel, compare);
    }

    /**
     * @brief get timer0 channel counter value
     * @param channel timer0 channel to get counter value
     * @return current counter value
     * @note the channel must be start()-ed before calling this function
     */
    TIMER0_INLINE_ATTR uint16_t getCount()
    {
        //CORE_ASSERT(this->isStarted, "Timer0::getCount(): channel not initialized");
        return TIMER0_GetCntReg(this->config->peripheral.register_base, this->config->peripheral.channel);
    }

    /**
     * @brief set timer0 channel callback priority
     * @param channel timer0 channel to set callback priority for
     * @param priority priority to set
     */
    TIMER0_INLINE_ATTR void setCallbackPriority(const uint32_t priority)
    {
        if (!this->isStarted)
        {
            CORE_ASSERT_FAIL("Timer0::setCallbackPriority(): channel not initialized");
            return;
        }

        NVIC_SetPriority(this->config->interrupt.interrupt_number, priority);
    }

    /**
     * @brief clear timer0 channel interrupt flag
     * @note must be called in the interrupt handler to clear the interrupt flag.
     *       failure to do so may cause undefined behavior
     */
    TIMER0_INLINE_ATTR void clearInterruptFlag()
    {
        TIMER0_ClearFlag(this->config->peripheral.register_base, this->config->peripheral.channel);
    }

private:
    timer0_channel_config_t *config;
    voidFuncPtr callback;
    bool isStarted = false;
};
