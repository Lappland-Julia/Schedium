cmake_minimum_required(VERSION 4.2)
project(Schedium)

set(CMAKE_CXX_STANDARD 26)

add_executable(Schedium main.cpp
        src/ScheduleEntry.cpp
        src/ScheduleEntry.h
        src/ScheduleManager.cpp
        src/ScheduleManager.h
        src/Generators.h
        src/Validator.cpp
        src/Validator.h
        benchmarks/ValidatorChecker.cpp
        benchmarks/ValidatorChecker.h)
