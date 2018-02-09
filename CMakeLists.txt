cmake_minimum_required(VERSION 3.5)
project(funky-as)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DVM_ARCH_BITS=32)

add_library(funky-as
        assembler.c
        instructions.c
        instructions.h
        vm_arch.h main.c assembler.h constants.c constants.h)

add_executable(funky-as-bin main.c)
target_link_libraries(funky-as-bin funky-as)
set_target_properties(funky-as-bin PROPERTIES OUTPUT_NAME funky-as)