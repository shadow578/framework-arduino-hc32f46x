#include "timer0_config.h"

timer0_channel_config_t TIMER01A_config = {
    .peripheral = {
        .register_base = M4_TMR01,
        .channel = Tim0_ChannelA,
        .clock_id = PWC_FCG2_PERIPH_TIM01,
    },
    .interrupt = {
        .interrupt_source = INT_TMR01_GCMA,
    },
};

timer0_channel_config_t TIMER01B_config = {
    .peripheral = {
        .register_base = M4_TMR01,
        .channel = Tim0_ChannelB,
        .clock_id = PWC_FCG2_PERIPH_TIM01,
    },
    .interrupt = {
        .interrupt_source = INT_TMR01_GCMB,
    },
};

timer0_channel_config_t TIMER02A_config = {
    .peripheral = {
        .register_base = M4_TMR02,
        .channel = Tim0_ChannelA,
        .clock_id = PWC_FCG2_PERIPH_TIM02,
    },
    .interrupt = {
        .interrupt_source = INT_TMR02_GCMA,
    },
};

timer0_channel_config_t TIMER02B_config = {
    .peripheral = {
        .register_base = M4_TMR02,
        .channel = Tim0_ChannelB,
        .clock_id = PWC_FCG2_PERIPH_TIM02,
    },
    .interrupt = {
        .interrupt_source = INT_TMR02_GCMB,
    },
};
