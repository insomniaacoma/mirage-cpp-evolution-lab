cmake_minimum_required(VERSION 3.25)
project(mirage_m0 C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# M0: single game target, flat src/ (charter P0). Module splits wait for real pressure.
add_executable(mirage_m0 src/main.cpp src/world_graph.cpp)

# ADR-0002: raylib call surface is confined to src/main.cpp and src/scene.cpp.
set(RAYLIB_ROOT "${CMAKE_SOURCE_DIR}/third_party/raylib" CACHE PATH "prebuilt raylib (tools/fetch_raylib.py)")
target_include_directories(mirage_m0 PRIVATE src "${RAYLIB_ROOT}/include")
target_link_directories(mirage_m0 PRIVATE "${RAYLIB_ROOT}/lib")
target_link_libraries(mirage_m0 PRIVATE raylib opengl32 gdi32 winmm user32 shell32 imm32 setupapi)

# Headless data-structure tests (ISS-005). No raylib dependency here.
add_executable(test_worldgraph tests/test_worldgraph.cpp src/world_graph.cpp)
target_include_directories(test_worldgraph PRIVATE src)
