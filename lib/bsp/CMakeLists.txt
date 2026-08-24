set(BSP_LIB BSP)
add_library(${BSP_LIB} STATIC)

target_link_libraries(${BSP_LIB} PRIVATE
    PLATFORM
)

target_link_libraries(${BSP_LIB} PUBLIC
    common
)

target_include_directories(${BSP_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_sources(${BSP_LIB} PRIVATE
    bsp.c
)
