cmake_minimum_required(VERSION 3.5)

project(stm32_template C)

set(TARGET_NAME ${PROJECT_NAME})

# === Toolchain ===
set(ARM_TOOLCHAIN_PATH $ENV{ARM_TOOLCHAIN_PATH})
set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc)
set(OBJCOPY ${ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy)
set(STFLASH st-flash)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# === Compiler flags ===
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -g3 -Os -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -nostdlib")

# === Define STM32 family for libopencm3 ===
add_definitions(-DSTM32F1)

# === Linker flags ===
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld)

set(CMAKE_EXE_LINKER_FLAGS
        "-T${LINKER_SCRIPT} -nostartfiles -Wl,--gc-sections -Wl,-Map=${TARGET_NAME}.map")

# === Include libopencm3 headers ===
include_directories(
        ${CMAKE_SOURCE_DIR}/libopencm3/include
)

# === Link libopencm3 static library ===
link_directories(
        ${CMAKE_SOURCE_DIR}/libopencm3/lib
)

# === Source files ===
set(SOURCE_FILES
        main.c
)

# === Output paths ===
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# === Build ELF ===
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# libopencm3 library name for STM32F1
target_link_libraries(${TARGET_NAME} opencm3_stm32f1)

# === Outputs ===
set(BIN_TARGET "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.bin")
set(HEX_TARGET "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.hex")
set(ELF_TARGET "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.elf")

add_custom_command(OUTPUT ${BIN_TARGET}
        COMMAND ${OBJCOPY} -O binary ${ELF_TARGET} ${BIN_TARGET}
        DEPENDS ${ELF_TARGET})

add_custom_target(bin ALL DEPENDS ${BIN_TARGET})

add_custom_command(OUTPUT ${HEX_TARGET}
        COMMAND ${OBJCOPY} -O ihex ${ELF_TARGET} ${HEX_TARGET}
        DEPENDS ${ELF_TARGET})

add_custom_target(hex ALL DEPENDS ${HEX_TARGET})

# === Flash ===
add_custom_target(flash
        COMMAND ${STFLASH} write ${BIN_TARGET} 0x8000000
        DEPENDS ${BIN_TARGET}
)
