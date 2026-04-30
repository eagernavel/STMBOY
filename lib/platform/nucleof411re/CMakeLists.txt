set(PLATFORM_NUCLEOF411RE_TARGET PLATFORM_NUCLEOF411RE)
add_library(${PLATFORM_NUCLEOF411RE_TARGET} STATIC)

target_link_libraries(${PLATFORM_NUCLEOF411RE_TARGET} PUBLIC 
    COMPONENT_ILI9486
    STM32_LL_DRIVER
)

target_include_directories(${PLATFORM_NUCLEOF411RE_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_sources(${PLATFORM_NUCLEOF411RE_TARGET} PRIVATE
    platform_nucleof411re.c
    platform_nucleof411re_clock.c
    platform_nucleof411re_serial.c
    platform_nucleof411re_ili9486.c
    platform_nucleof411re_buttons.c
    systick.c
)
