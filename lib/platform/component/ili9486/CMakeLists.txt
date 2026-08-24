add_library(COMPONENT_ILI9486 STATIC)
target_include_directories(COMPONENT_ILI9486 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(COMPONENT_ILI9486 PUBLIC common)
target_sources(COMPONENT_ILI9486 PRIVATE ili9486.c)
