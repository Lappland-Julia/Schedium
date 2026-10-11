find_program(SCHEDIUM_CLANG_FORMAT NAMES clang-format-18 clang-format)
find_program(SCHEDIUM_CLANG_TIDY NAMES clang-tidy-18 clang-tidy)

file(GLOB_RECURSE SCHEDIUM_FORMAT_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
    "${PROJECT_SOURCE_DIR}/tests/*.hpp"
)
if(SCHEDIUM_BUILD_BENCHMARKS)
    file(GLOB_RECURSE SCHEDIUM_BENCHMARK_FORMAT_SOURCES CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/benchmarks/*.cpp"
        "${PROJECT_SOURCE_DIR}/benchmarks/*.hpp"
        "${PROJECT_SOURCE_DIR}/benchmarks/*.h"
    )
    list(APPEND SCHEDIUM_FORMAT_SOURCES ${SCHEDIUM_BENCHMARK_FORMAT_SOURCES})
endif()
list(SORT SCHEDIUM_FORMAT_SOURCES)

if(SCHEDIUM_CLANG_FORMAT)
    add_custom_target(format
        COMMAND "${SCHEDIUM_CLANG_FORMAT}" --style=file -i ${SCHEDIUM_FORMAT_SOURCES}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Formatting C++ sources with clang-format"
        VERBATIM
    )
    add_custom_target(format-check
        COMMAND "${SCHEDIUM_CLANG_FORMAT}" --style=file --dry-run --Werror ${SCHEDIUM_FORMAT_SOURCES}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Checking C++ formatting with clang-format"
        VERBATIM
    )
endif()

if(SCHEDIUM_CLANG_TIDY AND CMAKE_GENERATOR MATCHES "Makefiles|Ninja")
    get_target_property(SCHEDIUM_LIBRARY_SOURCES Schedium SOURCES)
    list(FILTER SCHEDIUM_LIBRARY_SOURCES INCLUDE REGEX "\\.cpp$")
    add_custom_target(clang-tidy
        COMMAND "${SCHEDIUM_CLANG_TIDY}" -p "${PROJECT_BINARY_DIR}" ${SCHEDIUM_LIBRARY_SOURCES}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Analyzing the Schedium library with clang-tidy"
        VERBATIM
    )
endif()
