set(BSP_LIB BSP)
add_library(${BSP_LIB} INTERFACE)

target_link_libraries(${BSP_LIB} INTERFACE 
    PLATFORM
)

target_include_directories(${BSP_LIB} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_sources(${BSP_LIB} INTERFACE
    bsp.c
)
