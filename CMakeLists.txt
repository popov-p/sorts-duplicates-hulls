cmake_minimum_required(VERSION 3.21.0)

project(nppame
    VERSION 1.0.0
    DESCRIPTION "Вступительное задание (задания 1,2 и 3)"
    LANGUAGES CXX
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(sorting_algorithms_static_lib sorting-algorithms)
set(hull_building_demo_static_lib hull-building-demo)

add_subdirectory(src)
add_subdirectory(examples)
