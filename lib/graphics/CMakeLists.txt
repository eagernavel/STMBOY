cmake_minimum_required(VERSION 3.13)

# Define the graphics library
add_library(graphics STATIC
    stm32boy.c
)

# Add include directories to find "platform/..." includes
target_include_directories(graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)