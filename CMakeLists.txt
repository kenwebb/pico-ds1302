cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ds1302pico_ex01 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(ds1302pico_ex01
  ds1302pico_ex01.c
  ds1302.c
)

pico_enable_stdio_usb(ds1302pico_ex01 1)
pico_enable_stdio_uart(ds1302pico_ex01 1)

pico_add_extra_outputs(ds1302pico_ex01)

target_link_libraries(ds1302pico_ex01 pico_stdlib)
