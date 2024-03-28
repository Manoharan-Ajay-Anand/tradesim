cmake_minimum_required(VERSION 3.5)

project(tradesim
    VERSION 1.0
    DESCRIPTION "tradesim app"
    LANGUAGES CXX)

set(CMAKE_INSTALL_SRVDIR /srv CACHE PATH "Path to install web files")

include(CTest)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    add_subdirectory(libs)
endif()

add_subdirectory(src)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS tradesim)
    install(DIRECTORY srv/
            DESTINATION ${CMAKE_INSTALL_SRVDIR})
endif()

if (BUILD_TESTING)
    add_subdirectory(test)
    target_include_directories(tradesim_test PRIVATE src)
endif()
