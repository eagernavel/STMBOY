cmake_minimum_required(VERSION 3.13)

add_library(game_engine STATIC
    animation.c
    actor.c
)

target_include_directories(game_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(game_engine PUBLIC common)
