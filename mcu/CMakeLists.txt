cmake_minimum_required(VERSION 3.12)

set(NAME wetterstation)

include(stm32h7_import.cmake)
include(libdaisy_import.cmake)

project(${NAME} C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

enable_language(C CXX)

add_executable(${NAME}
        src/run.cpp)

target_link_libraries(${NAME} libdaisy)