cmake_minimum_required(VERSION 3.20)
project(malloc_dump)

set(CMAKE_CXX_STANDARD 20)

include_directories(inject)
add_executable(malloc_dump main.cpp)

add_library(injector inject/Ptrace.cpp inject/Ptrace.h inject/android_malloc.h)

target_link_libraries(malloc_dump
        PRIVATE injector)
