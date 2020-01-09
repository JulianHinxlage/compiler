cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)
set(FLAGS "-pthread -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

project(compiler)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

set(UTIL_PATH "../util")
include_directories(src)
include_directories(${UTIL_PATH}/include)

link_directories(compiler ${UTIL_PATH}/lib)
add_executable(compiler ${SOURCES} ${HEADERS})
target_link_libraries(compiler util)
