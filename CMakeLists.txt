cmake_minimum_required(VERSION 3.10)

project(MIO
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(MIOTest testd)
else()
    set(MIOTest test)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(${MIOTest}
	${CMAKE_SOURCE_DIR}/test.cpp)

