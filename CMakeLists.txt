cmake_minimum_required(VERSION 3.5)
project(assembler)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DVM_ARCH_BITS=32)

add_library(assembler
        assembler.c
        instructions.c
        instructions.h
        vm_arch.h main.c assembler.h)

add_executable(assembler-bin main.c)
target_link_libraries(assembler-bin assembler)
set_target_properties(assembler-bin PROPERTIES OUTPUT_NAME assembler)