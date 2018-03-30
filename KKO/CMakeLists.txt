cmake_minimum_required(VERSION 3.9)
project(gif2bmp)

set_source_files_properties(gif2bmp.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(main.c PROPERTIES LANGUAGE CXX)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")

add_executable(kko main.c gif2bmp.c gif2bmp.h)