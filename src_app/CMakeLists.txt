
#
# App
#

uni_hal_add_executable(application)

target_sources(application PRIVATE
    main.c
    led.c

    config/config_rcc.c
    config/config_uart.c
)

target_include_directories(application PRIVATE .)

target_link_libraries(application PRIVATE mbedcrypto)