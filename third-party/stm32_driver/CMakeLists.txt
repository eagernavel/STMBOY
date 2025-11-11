set(HAL_LIB STM32_LL_DRIVER)
add_library(${HAL_LIB} INTERFACE)

###############
#    CMSIS    #
###############

target_include_directories(${HAL_LIB} INTERFACE
    cmsis/core
    cmsis/device/include
)

target_sources(${HAL_LIB} INTERFACE
    cmsis/device/source/system_stm32f4xx.c
)

if(${STM32F4xx_TARGET} STREQUAL "STM32F411xE")
    target_sources(${HAL_LIB} INTERFACE
        cmsis/device/startup/startup_stm32f411xe.s
    )
elseif(${STM32F4xx_TARGET} STREQUAL "STM32F412Zx")
    target_sources(${HAL_LIB} INTERFACE
        cmsis/device/startup/startup_stm32f412zx.s
    )
else()
    message(FATAL_ERROR "Unsupported STM32F4xx_TARGET: ${STM32F4xx_TARGET}. Currently supported options: STM32F411xE, STM32F412Zx")
endif()

target_compile_definitions(${HAL_LIB} INTERFACE ${STM32F4xx_TARGET})

###############
#  LL DRIVER  #
###############
target_include_directories(${HAL_LIB} INTERFACE
    stm32_ll_drivers/include
)

target_sources(${HAL_LIB} INTERFACE
    stm32_ll_drivers/source/stm32f4xx_ll_exti.c
    stm32_ll_drivers/source/stm32f4xx_ll_gpio.c
    stm32_ll_drivers/source/stm32f4xx_ll_pwr.c
    stm32_ll_drivers/source/stm32f4xx_ll_rcc.c
    stm32_ll_drivers/source/stm32f4xx_ll_tim.c
    stm32_ll_drivers/source/stm32f4xx_ll_usart.c
    stm32_ll_drivers/source/stm32f4xx_ll_utils.c
)

target_compile_definitions(${HAL_LIB} INTERFACE
    USE_FULL_ASSERT
    USE_FULL_LL_DRIVER
)
