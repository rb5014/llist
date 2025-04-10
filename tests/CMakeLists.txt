# Test configuration
enable_testing()

# Set C standard and warnings
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Find testing frameworks (optional but recommended)
find_package(CUnit QUIET)  # Or other testing framework

# Test executable with proper source file grouping
file(GLOB TEST_SOURCES "*.c" "*.cpp")  # Replace with test file patterns
add_executable(test_llist ${TEST_SOURCES})

# Google Test Integration (optional)
find_package(GTest)
if(GTest_FOUND)
    target_link_libraries(test_llist PRIVATE GTest::GTest GTest::Main)
else()
    message(STATUS "GTest not found - using simple test runner")
endif()

# Link against main library and testing frameworks
target_link_libraries(test_llist
    PRIVATE
        llist
        # Add testing framework if found
        $<$<BOOL:${CUNIT_FOUND}>:CUnit::CUnit>
)

# Include directories with modern CMake practice
target_include_directories(test_llist
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
    PUBLIC
        $<TARGET_PROPERTY:llist,INTERFACE_INCLUDE_DIRECTORIES>
)

# Configure test output
set(CMAKE_CTEST_ARGUMENTS
    "--output-on-failure"
    "--verbose"
    "--timeout" "60"  # Add test timeout
)

# Windows-specific DLL handling
if(WIN32)
    # Copy DLL to test directory
    add_custom_command(TARGET test_llist POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:llist>"
        "$<TARGET_FILE_DIR:test_llist>"
        COMMENT "Copying DLL to test directory"
    )
    
    # For MSVC builds, copy PDB files if they exist
    if(MSVC)
        add_custom_command(TARGET test_llist POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_PDB_FILE:llist>"
            "$<TARGET_FILE_DIR:test_llist>"
            COMMENT "Copying PDB to test directory"
        )
    endif()
endif()

# Register tests with labels and properties
add_test(
    NAME llist_core_tests
    COMMAND test_llist
)

# Set test properties
set_tests_properties(llist_core_tests PROPERTIES
    TIMEOUT 60
    LABELS "core"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add memory check if available
find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND)
    add_test(
        NAME llist_memcheck
        COMMAND ${MEMORYCHECK_COMMAND} --leak-check=full ./test_llist
    )
    set_tests_properties(llist_memcheck PROPERTIES
        LABELS "memory;slow"
    )
endif()

# Add installation target for tests (optional)
install(TARGETS test_llist
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests
)