cmake_minimum_required(VERSION 3.10)

project(MultiplatformMmapIO)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

add_executable(test
	${CMAKE_SOURCE_DIR}/MultiplatformMmapIO/MultiplatformMmapIO.cpp)

