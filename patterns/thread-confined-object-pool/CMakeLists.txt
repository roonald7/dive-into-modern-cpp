cmake_minimum_required(VERSION 3.16)
project(LowLatencyBenchmarks LANGUAGES CXX)

# Force modern C++ standard compliance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check for build type; default to Release for accurate benchmarks
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Define the executable
add_executable(pipeline_benchmark main.cpp)

# --- Microarchitectural & Optimization Flags ---
if(MSVC)
    target_compile_options(pipeline_benchmark PRIVATE
        /O2                 # Maximize speed
        /Oi                 # Enable intrinsic functions
        /Ot                 # Favor fast code
        /W4                 # High warning level
        /permissive-        # Strict standard compliance
    )
else()
    target_compile_options(pipeline_benchmark PRIVATE
        -O3                 # Aggressive optimization level
        -march=native       # Target instructions of the host CPU (Critical for RDTSC/vectorization)
        -Wall -Wextra       # Strict warning compliance
        -pedantic
        -fno-plt            # Optimization: Bypass Procedure Linkage Table overhead
    )
    
    # Enable Link Time Optimization / Interprocedural Optimization (LTO/IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT error)
    if(ipo_supported)
        set_property(TARGET pipeline_benchmark PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "Link Time Optimization (LTO/IPO) enabled.")
    endif()
endif()

# --- Platform-Specific Threading & Libraries ---
find_package(Threads REQUIRED)
target_link_libraries(pipeline_benchmark PRIVATE Threads::Threads)

# On Linux, explicitly ensure real-time/posix threading compatibility if needed
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(pipeline_benchmark PRIVATE rt)
endif()