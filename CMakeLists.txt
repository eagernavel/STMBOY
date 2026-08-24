cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(STM32BOY_BUILD_HOST_TESTS "Build native host tests" OFF)

if(STM32BOY_BUILD_HOST_TESTS)
    project(stm32boy-host-tests LANGUAGES C)
    enable_testing()
    add_subdirectory(tests)
    return()
endif()

project(nucleo-tft-ili9486
    LANGUAGES ASM C
    VERSION 1.0.0
)

add_subdirectory(third-party/stm32_driver)
add_subdirectory(lib)
add_subdirectory(source)
