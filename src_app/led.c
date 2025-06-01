#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <uni_hal.h>
#include "led.h"

#include "mbedtls/md.h"
#include <string.h>

#include "stm32l4xx_hal_cortex.h"
#include "stm32l4xx_hal_flash.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_rcc.h"

// Конфигурация памяти (из datasheet STM32L496GR)
#define FLASH_BASE_ADDR        0x08000000
#define FLASH_SIZE             (1024 * 1024)   // 1 MB
#define FLASH_PAGE_SIZE        2048            // 2 KB
#define HMAC_STORAGE_ADDR      (FLASH_BASE_ADDR + FLASH_SIZE - 256) // Последние 256 байт

uni_hal_gpio_pin_context_t g_led_pin = {
   .gpio_bank = UNI_HAL_CORE_PERIPH_GPIO_B,
   .gpio_pin = UNI_HAL_GPIO_PIN_14,
   .gpio_pull = UNI_HAL_GPIO_PULL_NO,
   .gpio_type = UNI_HAL_GPIO_TYPE_OUT_PP,
   .alternate = UNI_HAL_GPIO_ALTERNATE_0
};

// Ключ должен храниться в защищенной памяти (например, в OTP)
static const uint8_t hmac_key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                     0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

// Функция вычисления HMAC-SHA256
bool verify_firmware_hmac() {
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *md_info;
    uint8_t hmac_calculated[32];

    // 1. Инициализация HMAC-контекста
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_md_init(&ctx);
    if(mbedtls_md_setup(&ctx, md_info, 1) != 0) {
        return false;
    }

    // 2. Старт HMAC с ключом
    mbedtls_md_hmac_starts(&ctx, hmac_key, sizeof(hmac_key));

    // 3. Обход флеш-памяти (исключая последнюю страницу)
    const uint32_t flash_end = HMAC_STORAGE_ADDR - 1;

    for (uint32_t addr = FLASH_BASE_ADDR; addr < flash_end; addr += 4) {
        uint32_t data = *(volatile uint32_t*)addr;
        mbedtls_md_hmac_update(&ctx, (const uint8_t*)&data, sizeof(data));
    }

    // 4. Финализация вычислений
    mbedtls_md_hmac_finish(&ctx, hmac_calculated);
    mbedtls_md_free(&ctx);

    // 5. Сравнение с хранимым значением
    const uint8_t* stored_hmac = (const uint8_t*)HMAC_STORAGE_ADDR;
    return memcmp(hmac_calculated, stored_hmac, 32) == 0;
}

// Функция записи HMAC во флеш (для прошивки)
void write_hmac_to_flash() {
    // Вычисляем HMAC текущей прошивки
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    uint8_t hmac[32];

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, md_info, 1);
    mbedtls_md_hmac_starts(&ctx, hmac_key, sizeof(hmac_key));

    for (uint32_t addr = FLASH_BASE_ADDR; addr < HMAC_STORAGE_ADDR; addr += 4) {
        uint32_t data = *(volatile uint32_t*)addr;
        mbedtls_md_hmac_update(&ctx, (const uint8_t*)&data, sizeof(data));
    }

    mbedtls_md_hmac_finish(&ctx, hmac);
    mbedtls_md_free(&ctx);

    // Запись во флеш (требует предварительной разблокировки)
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);

    // Стирание последней страницы
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Page = (HMAC_STORAGE_ADDR - FLASH_BASE_ADDR) / FLASH_PAGE_SIZE,
        .NbPages = 1
    };
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase, &page_error);

    // Программирование HMAC
    for (int i = 0; i < 32; i += 8) {
        uint64_t data = *(uint64_t*)&hmac[i];
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, HMAC_STORAGE_ADDR + i, data);
    }

    HAL_FLASH_Lock();
}

// UART инициализация (пример для USART2)
void uart_init() {
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {
        .Pin = GPIO_PIN_2 | GPIO_PIN_3,
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Alternate = GPIO_AF7_USART2
    };
    HAL_GPIO_Init(GPIOA, &gpio);

    // USART_HandleTypeDef huart2 = {
    //     .Instance = USART2,
    //     .Init = {
    //         .BaudRate = 115200,
    //         .WordLength = USART_WORDLENGTH_8B,
    //         .StopBits = USART_STOPBITS_1,
    //         .Parity = USART_PARITY_NONE,
    //         .Mode = USART_MODE_TX_RX,
    //         .OverSampling = USART_OVERSAMPLING_16
    //     }
    // };
    // HAL_USART_Init(&huart2);

    // Перенаправление printf
    setvbuf(stdout, NULL, _IONBF, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void led_task(void* pvparameters) {
    (void)pvparameters;
    uni_hal_gpio_pin_init(&g_led_pin);
    uart_init();  // Инициализация UART

    while (true) {
        if (verify_firmware_hmac()) {
            uni_hal_gpio_pin_set(&g_led_pin, 1);  // Горит - прошивка цела
        } else {
            uni_hal_gpio_pin_set(&g_led_pin, 0);  // Потух - ошибка
            printf("ALARM! Firmware compromised!\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void led_init(){
    xTaskCreate(led_task, "IntegrityCheck", 1024, NULL, 5, NULL);
}