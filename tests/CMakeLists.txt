function(add_stm32boy_host_test target)
    add_executable(${target} ${ARGN})
    target_include_directories(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/graphics
        ${CMAKE_SOURCE_DIR}/source
    )
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_stm32boy_host_test(test_graphics
    test_graphics.c
    ${CMAKE_SOURCE_DIR}/lib/graphics/stm32boy.c
)

add_stm32boy_host_test(test_demo_model
    test_demo_model.c
    ${CMAKE_SOURCE_DIR}/source/demo/demo_model.c
)

add_stm32boy_host_test(test_ili9486
    test_ili9486.c
    ${CMAKE_SOURCE_DIR}/lib/platform/component/ili9486/ili9486.c
)

add_executable(demo_preview
    demo_preview.c
    ${CMAKE_SOURCE_DIR}/lib/graphics/stm32boy.c
    ${CMAKE_SOURCE_DIR}/source/demo/demo_model.c
    ${CMAKE_SOURCE_DIR}/source/demo/demo_view.c
)
target_include_directories(demo_preview PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/lib/graphics
    ${CMAKE_SOURCE_DIR}/source
)
target_compile_options(demo_preview PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME demo_preview
    COMMAND demo_preview ${CMAKE_BINARY_DIR}/demo-preview.bmp
)
