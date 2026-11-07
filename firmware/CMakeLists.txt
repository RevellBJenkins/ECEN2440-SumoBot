cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(ECEN2440_SumoBot C CXX ASM)
pico_sdk_init()

add_executable(ECEN2440_SumoBot
    src/main.cpp
)

target_link_libraries(ECEN2440_SumoBot
    pico_stdlib
    hardware_pwm
    hardware_i2c
)

pico_add_extra_outputs(ECEN2440_SumoBot)
