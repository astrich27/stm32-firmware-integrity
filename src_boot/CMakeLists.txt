
#
# Boot
#

uni_hal_add_executable(bootloader)

target_sources(bootloader PRIVATE
        main.c

        config/config_rcc.c
        config/config_uart.c
)

target_include_directories(bootloader PRIVATE .)
