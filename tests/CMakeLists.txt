add_executable(CMakeTests CMakeTests.cpp)
target_link_libraries(CMakeTests PRIVATE Schedium)

# Keep the test executable beside the DLL for Windows shared-library builds.
set_target_properties(CMakeTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
add_test(NAME CMakeTests COMMAND CMakeTests)
set_tests_properties(CMakeTests PROPERTIES TIMEOUT 30)

if(MINGW)
    # Resolve MinGW runtime DLLs even when the compiler is outside PATH.
    get_filename_component(compiler_directory "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set_tests_properties(CMakeTests PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${compiler_directory}"
    )
endif()
