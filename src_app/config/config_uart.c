//
// Includes
//

// ox app
#include "config/config_uart.h"



//
// Globals/Pins
//

#if 1
uni_hal_gpio_pin_context_t g_app_stdio_pin_rx_ctx = {
    .gpio_bank = UNI_HAL_CORE_PERIPH_GPIO_G,
    .gpio_pin = UNI_HAL_GPIO_PIN_8,
    .gpio_pull = UNI_HAL_GPIO_PULL_NO,
    .gpio_type = UNI_HAL_GPIO_TYPE_ALTERNATE_PP,
    .alternate = UNI_HAL_GPIO_ALTERNATE_8,
};

uni_hal_gpio_pin_context_t g_app_stdio_pin_tx_ctx = {
    .gpio_bank = UNI_HAL_CORE_PERIPH_GPIO_G,
    .gpio_pin = UNI_HAL_GPIO_PIN_7,
    .gpio_pull = UNI_HAL_GPIO_PULL_NO,
    .gpio_type = UNI_HAL_GPIO_TYPE_ALTERNATE_PP,
    .alternate = UNI_HAL_GPIO_ALTERNATE_8,
};
#endif

#if 0
uni_hal_gpio_pin_context_t g_app_stdio_pin_rx_ctx = {
    .gpio_bank = UNI_HAL_CORE_PERIPH_GPIO_A,
    .gpio_pin = UNI_HAL_GPIO_PIN_1,
    .gpio_pull = UNI_HAL_GPIO_PULL_NO,
    .gpio_type = UNI_HAL_GPIO_TYPE_ALTERNATE_PP,
    .alternate = UNI_HAL_GPIO_ALTERNATE_8,
};

uni_hal_gpio_pin_context_t g_app_stdio_pin_tx_ctx = {
    .gpio_bank = UNI_HAL_CORE_PERIPH_GPIO_A,
    .gpio_pin = UNI_HAL_GPIO_PIN_0,
    .gpio_pull = UNI_HAL_GPIO_PULL_NO,
    .gpio_type = UNI_HAL_GPIO_TYPE_ALTERNATE_PP,
    .alternate = UNI_HAL_GPIO_ALTERNATE_8,
};
#endif

//
// Globals/IO
//

UNI_HAL_IO_DEFINITION(g_app_stdio_io, 128, 512);



//
// Globals/UART
//

uni_hal_usart_context_t g_app_uart_ctx = {
    .instance = UNI_HAL_CORE_PERIPH_LPUART_1,
    .io = &g_app_stdio_io_ctx,
    .initialized = false,
    .in_transmission = false,
    .pin_rx = &g_app_stdio_pin_rx_ctx,
    .pin_tx = &g_app_stdio_pin_tx_ctx,
};
