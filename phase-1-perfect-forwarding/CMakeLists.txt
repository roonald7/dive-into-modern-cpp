cmake_minimum_required(VERSION 3.10)
project(PerfectForwardingPhase1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Executables
add_executable(basic_example src/basic_example.cpp)
add_executable(production_example src/production_example.cpp)

# Tests
add_executable(forwarding_tests tests/test_main.cpp)
