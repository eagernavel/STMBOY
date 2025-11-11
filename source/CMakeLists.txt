set(EXECUTABLE_TARGET firmware)
add_executable(${EXECUTABLE_TARGET})

target_link_libraries(${EXECUTABLE_TARGET} PRIVATE
    BSP
)

# Add compile options here
target_compile_options(${EXECUTABLE_TARGET} PRIVATE
    -Wall -Wextra -Wpedantic
)

# Syscalls
target_sources(${EXECUTABLE_TARGET} PRIVATE
    syscalls.c
    sysmem.c
)

# Source code
target_include_directories(${EXECUTABLE_TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_sources(${EXECUTABLE_TARGET} PRIVATE
    main.c
    assert.c
)

# Generate a symbol map (with cross-reference information) from this executable. Note -Map option only works in GCC
target_link_options(${EXECUTABLE_TARGET} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_TARGET}.map -Wl,--cref)
# Add map to clean target
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES ADDITIONAL_CLEAN_FILES ${EXECUTABLE_TARGET}.map)

# Generate binaries form elf file after setup
add_custom_command(TARGET ${EXECUTABLE_TARGET} 
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE_TARGET}.elf ${EXECUTABLE_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE_TARGET}.elf ${EXECUTABLE_TARGET}.bin
)
