cmake_minimum_required(VERSION 3.19)
project(ex_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex_2 main.c)
target_link_libraries(ex_2 pthread)
target_link_libraries(ex_2 m)