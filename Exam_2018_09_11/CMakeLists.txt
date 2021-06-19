cmake_minimum_required(VERSION 3.19)
project(Exam_2018_09_11 C)

set(CMAKE_C_STANDARD 99)

add_executable(Exam_2018_09_11 main.c)
target_link_libraries(Exam_2018_09_11 pthread)