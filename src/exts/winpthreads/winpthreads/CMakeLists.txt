cmake_minimum_required(VERSION 3.1)

project(winpthreads)

file(GLOB _SRCS_ include/*.h
                 src/*.h
                 src/*.c
#                 src/libgcc/*.c
#                 src/libgcc/*.S
                 )

include_directories(include)

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/config.h)

add_definitions(-D__x86_64__)

add_library(winpthreads   SHARED ${_SRCS_})
add_library(winpthreads_a STATIC ${_SRCS_})

