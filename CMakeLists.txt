cmake_minimum_required(VERSION 3.5)
project(funky-as)

set(CMAKE_C_STANDARD 11)

add_definitions(-DVM_ARCH_BITS=32)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(include)

add_library(funky-as STATIC
        src/libfunkyas/funkyas.c
        src/libfunkyas/instructions.c
        src/libfunkyas/instructions.h
        src/libfunkyas/vm_arch.h src/main.c include/funkyas/funkyas.h src/libfunkyas/constants.c src/libfunkyas/constants.h src/libfunkyas/string_functions.c src/version.h)

add_executable(funky-as-bin src/main.c)
target_link_libraries(funky-as-bin funky-as)
set_target_properties(funky-as-bin PROPERTIES OUTPUT_NAME funky-as)
file(COPY include DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS funky-as-bin DESTINATION bin)
