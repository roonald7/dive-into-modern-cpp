cmake_minimum_required(VERSION 3.28)
project(ModulesPhase1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Note: As of CMake 3.28, modules are fully supported
add_executable(basic_example src/main.cpp)
target_sources(basic_example PUBLIC FILE_SET CXX_MODULES FILES src/Math.cppm)
