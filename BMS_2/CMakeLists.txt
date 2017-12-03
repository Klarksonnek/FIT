cmake_minimum_required(VERSION 2.8.11)
project(bms2 CXX)
add_definitions(-std=c++11 -Wall -pedantic -Wextra)

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/Common.cpp
	${PROJECT_SOURCE_DIR}/CustomException.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
)

add_library(BMS2Lib ${SOURCES})

message(${PROJECT_SOURCE_DIR})

add_executable("bms2A" ${PROJECT_SOURCE_DIR}/bms2A.cpp)
target_link_libraries("bms2A"
	-Wl,--whole-archive
	BMS2Lib
		${PROJECT_SOURCE_DIR}/libsndfile.a
	-Wl,--no-whole-archive
)

add_executable("bms2B" bms2B.cpp)
target_link_libraries("bms2B"
	-Wl,--whole-archive
	BMS2Lib
	${PROJECT_SOURCE_DIR}/libsndfile.a
	-Wl,--no-whole-archive
)
