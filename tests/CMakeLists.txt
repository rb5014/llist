# Enable detailed test output
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure --verbose")

# Test executable
add_executable(test_llist test_llist.c)

# Link against main library
target_link_libraries(test_llist PRIVATE llist)

# Include directories
target_include_directories(test_llist PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Windows-specific DLL handling
if(WIN32)
    add_custom_command(TARGET test_llist POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:llist>"  # Automatically resolves to libllist.dll
        "$<TARGET_FILE_DIR:test_llist>/$<TARGET_FILE_NAME:llist>"
        COMMENT "Copying $<TARGET_FILE_NAME:llist> to test directory"
    )
endif()

# Register test
add_test(NAME llist_tests COMMAND test_llist)