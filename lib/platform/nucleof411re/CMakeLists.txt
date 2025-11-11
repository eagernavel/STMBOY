set(PLATFORM_NUCLEOF411RE_TARGET PLATFORM_NUCLEOF411RE)
add_library(${PLATFORM_NUCLEOF411RE_TARGET} INTERFACE)

target_link_libraries(${PLATFORM_NUCLEOF411RE_TARGET} INTERFACE 
    COMPONENT_ILI9486
    STM32_LL_DRIVER
)

target_include_directories(${PLATFORM_NUCLEOF411RE_TARGET} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_sources(${PLATFORM_NUCLEOF411RE_TARGET} INTERFACE
    platform_nucleof411re.c
    platform_nucleof411re_clock.c
    platform_nucleof411re_serial.c
    platform_nucleof411re_ili9486.c
)
