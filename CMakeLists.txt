cmake_minimum_required(VERSION 3.25)
project(mirage_m0 C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# M0: single target, flat src/ (charter P0). Module splits wait for real pressure.
add_executable(mirage_m0 src/main.cpp src/world_graph.cpp)
target_include_directories(mirage_m0 PRIVATE src)

# Headless data-structure tests (ISS-005). No raylib dependency here.
add_executable(test_worldgraph tests/test_worldgraph.cpp src/world_graph.cpp)
target_include_directories(test_worldgraph PRIVATE src)
