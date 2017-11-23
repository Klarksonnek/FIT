cmake_minimum_required(VERSION 2.8.11)
project(p1 CXX)

find_library (POCO_FOUNDATION PocoFoundation)

add_definitions(-std=c++11 -Wall -pedantic -Wextra)

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/BTSLoadData.cpp
	${PROJECT_SOURCE_DIR}/Coordinates.cpp
	${PROJECT_SOURCE_DIR}/CustomException.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
)

add_library(ExampleCmake ${SOURCES})

add_executable(${PROJECT_NAME} main.cpp)

set(LIBS
	${POCO_FOUNDATION}
)

target_link_libraries(${PROJECT_NAME}
	-Wl,--whole-archive
	ExampleCmake
	-Wl,--no-whole-archive
	${LIBS}
)