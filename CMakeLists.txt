cmake_minimum_required(VERSION 3.7)
project(CHIP_8_Emulator)

set(SOURCE_FILES src/MainEmu.cpp)
add_executable(CHIP_8_Emulator ${SOURCE_FILES})

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(CHIP_8_Emulator ${SDL2_LIBRARIES})