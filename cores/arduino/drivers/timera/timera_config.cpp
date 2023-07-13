#include "timera_config.h"

timera_config_t TIMERA1_config = {
    .peripheral = {
        .register_base = M4_TMRA1,
        .clock_id = PWC_FCG2_PERIPH_TIMA1,
    },
    .overflow_interrupt = {
        .interrupt_source = INT_TMRA1_OVF,
    },
    .underflow_interrupt = {
        .interrupt_source = INT_TMRA1_UDF,
    },
    .compare_interrupt = {
        .interrupt_source = INT_TMRA1_CMP,
    },
};

timera_config_t TIMERA2_config = {
    .peripheral = {
        .register_base = M4_TMRA2,
        .clock_id = PWC_FCG2_PERIPH_TIMA2,
    },
    .overflow_interrupt = {
        .interrupt_source = INT_TMRA2_OVF,
    },
    .underflow_interrupt = {
        .interrupt_source = INT_TMRA2_UDF,
    },
    .compare_interrupt = {
        .interrupt_source = INT_TMRA2_CMP,
    },
};

timera_config_t TIMERA3_config = {
    .peripheral = {
        .register_base = M4_TMRA3,
        .clock_id = PWC_FCG2_PERIPH_TIMA3,
    },
    .overflow_interrupt = {
        .interrupt_source = INT_TMRA3_OVF,
    },
    .underflow_interrupt = {
        .interrupt_source = INT_TMRA3_UDF,
    },
    .compare_interrupt = {
        .interrupt_source = INT_TMRA3_CMP,
    },
};

timera_config_t TIMERA4_config = {
    .peripheral = {
        .register_base = M4_TMRA4,
        .clock_id = PWC_FCG2_PERIPH_TIMA4,
    },
    .overflow_interrupt = {
        .interrupt_source = INT_TMRA4_OVF,
    },
    .underflow_interrupt = {
        .interrupt_source = INT_TMRA4_UDF,
    },
    .compare_interrupt = {
        .interrupt_source = INT_TMRA4_CMP,
    },
};

timera_config_t TIMERA5_config = {
    .peripheral = {
        .register_base = M4_TMRA5,
        .clock_id = PWC_FCG2_PERIPH_TIMA5,
    },
    .overflow_interrupt = {
        .interrupt_source = INT_TMRA5_OVF,
    },
    .underflow_interrupt = {
        .interrupt_source = INT_TMRA5_UDF,
    },
    .compare_interrupt = {
        .interrupt_source = INT_TMRA5_CMP,
    },
};

timera_config_t TIMERA6_config = {
    .peripheral = {
        .register_base = M4_TMRA6,
        .clock_id = PWC_FCG2_PERIPH_TIMA6,
    },
    .overflow_interrupt = {
        .interrupt_source = INT_TMRA6_OVF,
    },
    .underflow_interrupt = {
        .interrupt_source = INT_TMRA6_UDF,
    },
    .compare_interrupt = {
        .interrupt_source = INT_TMRA6_CMP,
    },
};
