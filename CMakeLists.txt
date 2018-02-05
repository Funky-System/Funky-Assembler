cmake_minimum_required(VERSION 3.5)
project(assembler)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DVM_ARCH_BITS=32)

add_executable(assembler
        assembler.c
        instructions.c
        instructions.h
        vm_arch.h)
