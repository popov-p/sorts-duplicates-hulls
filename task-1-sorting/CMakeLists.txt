cmake_minimum_required(VERSION 3.14)

project(task-1-sorting
    VERSION 1.0.0
    DESCRIPTION "Сравнение эффективности работы сортировок"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")


set(sorting_comparison_executable sort-comparison-exec)
set(sorting_algorithms_static_lib sorting-algorithms)

add_subdirectory(src)
add_subdirectory(examples)
