cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(microboy_bringup C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(microboy_bringup
    main.c
)

target_link_libraries(microboy_bringup pico_stdlib hardware_clocks)

pico_enable_stdio_usb(microboy_bringup 1)
pico_enable_stdio_uart(microboy_bringup 0)

pico_add_extra_outputs(microboy_bringup)
