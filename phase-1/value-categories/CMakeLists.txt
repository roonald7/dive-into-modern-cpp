cmake_minimum_required(VERSION 3.10)
project(ValueCategoriesPhase1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Executables
add_executable(basic_example src/basic_example.cpp)
add_executable(production_example src/production_example.cpp)
add_executable(copy_elision_example src/copy_elision_example.cpp)

# Tests
add_executable(exercise_tests tests/test_main.cpp)

# Benchmarks (Placeholder for now)
# find_package(benchmark REQUIRED)
# add_executable(benchmarks benchmarks/bench_main.cpp)
# target_link_libraries(benchmarks benchmark::benchmark)
