cmake_minimum_required(VERSION 3.10)
project(llist LANGUAGES C)

# Platform-specific settings
if(WIN32)
    add_compile_definitions(BUILDING_DLL)
    # MSYS2 UCRT64 specific settings
    set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")
    set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")
    set(CMAKE_MAKE_PROGRAM "C:/msys64/ucrt64/bin/mingw32-make.exe")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    find_package(Threads REQUIRED)
endif()

# Library target
add_library(llist SHARED src/llist.c)
target_include_directories(llist PUBLIC include)

# Link pthread on Unix
if(NOT WIN32)
    target_link_libraries(llist PRIVATE Threads::Threads)
endif()

# Output naming
set_target_properties(llist PROPERTIES
    OUTPUT_NAME "llist"  # Just "llist" without lib prefix
    PREFIX ""  # Disable automatic "lib" prefix
    SUFFIX ".dll"  # Explicitly set suffix
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install rules
install(TARGETS llist
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(FILES include/llist.h DESTINATION include)

# Testing configuration
# Enable debug messages
set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)
option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()