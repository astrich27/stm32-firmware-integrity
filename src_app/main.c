//
// Includes
//

// stdlib
#include <stdio.h>

// uni_hal
#include <uni_hal.h>
#include <io/uni_hal_io_stdio.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>


// app
#include "config/config_rcc.h"
#include "config/config_uart.h"
#include "led.h"

//
// Globals
//

uint32_t __attribute__((section(".FREERTOS_HEAP"))) ucHeap[configTOTAL_HEAP_SIZE / sizeof(uint32_t)];

//
// Functions
//

int main(void) {
    // PWR
    uni_hal_pwr_init();

    // IRQ
    uni_hal_core_irq_init();

    // RCC
#if !defined(UNI_HAL_TARGET_MCU_PC)
    uni_hal_rcc_stm32l4_config_set(&g_app_rcc_config);
#endif

    uni_hal_rcc_init();

    // DWT
    uni_hal_dwt_init();

    // UART
    uni_hal_usart_init(&g_app_uart_ctx);

    // STDIO
    uni_hal_io_stdio_init(g_app_uart_ctx.io);

    // RCC-MCO
    //uni_hal_rcc_stm32_mco_enable(0, UNI_HAL_RCC_CLKSRC_SYSCLK, 4);

    led_init();

    // Start FreeRTOS
    vTaskStartScheduler();

    // Unreachable
    return 0;
}

