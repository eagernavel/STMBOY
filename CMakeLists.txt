cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)

project(nucleo-tft-ili9486
    LANGUAGES ASM C
    VERSION 1.0.0
)

add_subdirectory(third-party/stm32_driver)
add_subdirectory(lib)
add_subdirectory(source)