cmake_minimum_required(VERSION 3.5)

project(c_lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pedantic -Wall -Wextra -Werror -g")
include_directories(include)
add_subdirectory(examples)
enable_testing()
add_subdirectory(test)