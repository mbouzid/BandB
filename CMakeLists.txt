cmake_minimum_required(VERSION 3.14)
project(BandB)


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_DEPS=ON -std=c++11 -pthread -fopenmp -O3 ")
# flag -static
add_compile_options("-lemon")

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
