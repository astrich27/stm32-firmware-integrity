#pragma once

//
// Includes
//

// unimcu
#include <uni_hal.h>



//
// Globals
//

#if defined(STM32L4)
    extern uni_hal_rcc_stm32l4_config_t g_app_rcc_config;
#else
    #error  "unknown target"
#endif

