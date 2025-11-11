# CMake System variables
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain configuration
set(CMAKE_C_COMPILER                arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER              arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER              arm-none-eabi-gcc)
set(CMAKE_AR                        arm-none-eabi-gcc-ar)
set(CMAKE_OBJCOPY                   arm-none-eabi-objcopy)
set(CMAKE_SIZE                      arm-none-eabi-size)

# Prevent linking issues from test files
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Make all executable targets of type .elf when using this toolchain
set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

# Common flags. LD_FLAGS are to be added by specific device toolchain file
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

set(CMAKE_ASM_FLAGS_INIT
    "${CPU_FLAGS} -x assembler-with-cpp"
    CACHE
    INTERNAL "Default ASM compiler flags."
)

set(CMAKE_C_FLAGS_INIT
	"${CPU_FLAGS} -fdata-sections -ffunction-sections" 
	CACHE 
	INTERNAL "Default C compiler flags."
)

set(CMAKE_EXE_LINKER_FLAGS_INIT
	"${CPU_FLAGS} ${LD_FLAGS} -Wl,--gc-sections -Wl,--print-memory-usage"
	CACHE 
	INTERNAL "Default linker flags."
)