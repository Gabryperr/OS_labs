cmake_minimum_required(VERSION 3.17)
project(ex_5 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lrt")

add_executable(ex_5 main.c)
target_link_libraries(ex_5 rt)