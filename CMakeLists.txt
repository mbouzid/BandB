cmake_minimum_required(VERSION 3.17)
project(BandB)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -std=c++17 -pthread -fopenmp -O3")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(.)


if(OpenMP_FOUND)
    target_link_libraries(BandB OpenMP::OpenMP_C)
endif()


set(SOURCE_FILES generator/instance.cpp
        main.cpp
        solver/node.cpp
        solver/solver.cpp
        common/utils.cpp
        solver/solution.cpp
        solver/upper_bound.cpp
        solver/core.cpp)

add_executable(BandB ${SOURCE_FILES})

