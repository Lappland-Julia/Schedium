cmake_minimum_required(VERSION 4.2)

project(Schedium LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Schedium
        src/ScheduleEntry.cpp
        src/ScheduleManager.cpp
        src/Validator.cpp
        src/Schedium.h
)

target_include_directories(Schedium
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(SchediumBenchmarks
        benchmarks/main.cpp
        benchmarks/Benchmark.cpp
)

target_include_directories(SchediumBenchmarks
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks
)

target_link_libraries(SchediumBenchmarks
        PRIVATE
        Schedium
)

if (WIN32)
    target_link_libraries(SchediumBenchmarks
            PRIVATE
            Psapi
    )
endif()