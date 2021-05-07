cmake_minimum_required(VERSION 3.19)
project(ex_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex_1 main.c)
target_link_libraries(ex_1 pthread)