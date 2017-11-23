cmake_minimum_required(VERSION 2.8.11)
project(p1 CXX)

find_library (POCO_FOUNDATION PocoFoundation)

add_definitions(-std=c++11 -Wall -pedantic -Wextra)

file(GLOB SOURCES
)

include_directories(
	${PROJECT_SOURCE_DIR}
)

add_executable(${PROJECT_NAME} main.cpp)

set(LIBS
	${POCO_FOUNDATION}
)

target_link_libraries(${PROJECT_NAME}
	-Wl,--whole-archive
	-Wl,--no-whole-archive
	${LIBS}
)