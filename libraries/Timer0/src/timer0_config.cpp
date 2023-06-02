#include "timer0_config.h"
#include "timer0_handlers.h"

timer0_config_t TIMER01_config = {
    .peripheral = {
        .register_base = M4_TMR01,
        .clock_id = PWC_FCG2_PERIPH_TIM01,
    },
    .channel_a_interrupt = {
        .interrupt_source = INT_TMR01_GCMA,
        .interrupt_handler = TIMER0x_channel_a_irq<1>,
        .user_callback = NULL,
    },
    .channel_b_interrupt = {
        .interrupt_source = INT_TMR01_GCMB,
        .interrupt_handler = TIMER0x_channel_b_irq<1>,
        .user_callback = NULL,
    },
};

timer0_config_t TIMER02_config = {
    .peripheral = {
        .register_base = M4_TMR02,
        .clock_id = PWC_FCG2_PERIPH_TIM02,
    },
    .channel_a_interrupt = {
        .interrupt_source = INT_TMR02_GCMA,
        .interrupt_handler = TIMER0x_channel_a_irq<2>,
    },
    .channel_b_interrupt = {
        .interrupt_source = INT_TMR02_GCMB,
        .interrupt_handler = TIMER0x_channel_b_irq<2>,
    },
};
