# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Include FreeRTOS (submodule)
include(FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(Water_Time C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES src/*.c)

# Create executable
add_executable(Water_Time ${SOURCE_FILES})

pico_set_program_name(Water_Time "Water_Time")
pico_set_program_version(Water_Time "0.1")

# Enable UART/USB output
pico_enable_stdio_uart(Water_Time 1)
pico_enable_stdio_usb(Water_Time 1)

# Link libraries
target_link_libraries(Water_Time
    pico_stdlib
    hardware_flash
    hardware_watchdog
    hardware_adc
    hardware_sync
    pico_flash
    hardware_dma
    FreeRTOS-Kernel-Heap4
)

# Include directories
target_include_directories(Water_Time PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Add extra outputs (UF2, bin, etc.)
pico_add_extra_outputs(Water_Time)
