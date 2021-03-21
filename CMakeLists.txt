cmake_minimum_required(VERSION 3.12)
project(Parch)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(PSim)

add_executable(simulator main.cpp)
target_link_libraries(simulator SIMLIB)
target_include_directories(simulator PRIVATE PSim/include)
