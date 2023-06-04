#include "timer0_config.h"

#define TIMER0_COUNT 2
timer0_config_t *TIMER0x[TIMER0_COUNT] = {
    &TIMER01_config,
    &TIMER02_config,
};

#define IS_VALID_TIMER0x(x) ((x) >= 1 && (x) <= TIMER0_COUNT)
#define ASSERT_VALID_TIMER0x(x) static_assert(IS_VALID_TIMER0x(x), "TIMER0 number must be between 1 and TIMER0_COUNT")

template <uint8_t x>
static void TIMER0x_channel_a_irq(void)
{
    ASSERT_VALID_TIMER0x(x);
    timer0_config_t *timer0x = TIMER0x[x - 1];

    // clear the interrupt flag
    TIMER0_ClearFlag(timer0x->peripheral.register_base, Tim0_ChannelA);

    // call the user callback
    if (timer0x->channel_a_state.user_callback != NULL)
    {
        timer0x->channel_a_state.user_callback();
    }
}

template <uint8_t x>
static void TIMER0x_channel_b_irq(void)
{
    ASSERT_VALID_TIMER0x(x);
    timer0_config_t *timer0x = TIMER0x[x - 1];

    // clear the interrupt flag
    TIMER0_ClearFlag(timer0x->peripheral.register_base, Tim0_ChannelB);

    // call the user callback
    if (timer0x->channel_b_state.user_callback != NULL)
    {
        timer0x->channel_b_state.user_callback();
    }
}
