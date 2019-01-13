cmake_minimum_required(VERSION 3.13)
project(simplefs)

set(CMAKE_C_STANDARD 11)

add_executable(simplefs main.c simplefs.h consts.h simplefs.c filesystem.c filesystem.h patheval.c patheval.h)

target_link_libraries(simplefs m)
