cmake_minimum_required(VERSION 3.5)

project(tradesim
    VERSION 1.0
    DESCRIPTION "tradesim app"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_subdirectory(src)

install(TARGETS tradesim)
