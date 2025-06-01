//
// Includes
//

// orsha
#include "config/config_rcc.h"



//
// Globals
//

#if defined(STM32L4)

uni_hal_rcc_stm32l4_config_t g_app_rcc_config = {
        .hse_enable = true,
        .hse_bypass = false,
        .hse_css = true,
        .lse_enable = false,

        .pll[0] = {
                .m = 1,
                .n = 25,
                .p = 4,
                .q = 4,
                .r = 4,
        },

        .timeout = {
                .hse = 250,
                .hsi = 250,
                .lse = 250,
                .lsi = 250,
                .pll = 250
        }
};

#endif
