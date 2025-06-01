#
# Project
#

cmake_minimum_required(VERSION 3.26)
project(kursach LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(cmake/cpm.cmake)
include(cmake/cpm-mbedtls.cmake)

# Mbedtls

add_subdirectory(src_mbedtls)
include_directories(src_mbedtls/include)
target_link_libraries(your_target PRIVATE mbedtls)

#
# Options
#

add_definitions(-DUNI_HAL_CAN_USE_FREERTOS)
add_definitions(-DUNI_HAL_I2C_USE_FREERTOS)

#
# Unimcu
#

add_subdirectory(src_hal)
include(src_hal/uni.hal.cmake)

#
# App
#

add_subdirectory(src_boot)
add_subdirectory(src_app)

