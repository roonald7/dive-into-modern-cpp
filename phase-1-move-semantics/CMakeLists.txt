cmake_minimum_required(VERSION 3.10)
project(MoveSemanticsPhase1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Executables
add_executable(basic_example src/basic_example.cpp)
add_executable(production_example src/production_example.cpp)
add_executable(noexcept_bench src/noexcept_bench.cpp)

# Tests
add_executable(socket_exercise tests/socket_test.cpp)
