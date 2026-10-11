cmake_minimum_required(VERSION 3.22)

project(Schedium LANGUAGES CXX)

include(CTest)
option(SCHEDIUM_BUILD_BENCHMARKS "Build the Windows-only benchmarks" ${WIN32})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Schedium
        src/ScheduleEntry.cpp
        src/ScheduleManager.cpp
        src/Validator.cpp
        src/ScheduleEntry.hpp
        src/ScheduleManager.hpp
        src/Validator.hpp
        src/Schedium.hpp
)

target_compile_features(Schedium PUBLIC cxx_std_23)
set_target_properties(Schedium PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(Schedium
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(SCHEDIUM_BUILD_BENCHMARKS)
    if(NOT WIN32)
        message(FATAL_ERROR "SchediumBenchmarks requires Windows. Use -DSCHEDIUM_BUILD_BENCHMARKS=OFF.")
    endif()

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
            Psapi
    )
endif()

include(cmake/ClangTools.cmake)
