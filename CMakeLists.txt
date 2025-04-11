cmake_minimum_required(VERSION 3.10)

# Platform detection
if(WIN32)
  set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "")
endif()

# Project settings
project(llist
    LANGUAGES C
    VERSION 1.0.0  # Added version metadata
    DESCRIPTION "Lightweight Linked List Library"
)

# Set C standard and warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Platform-specific settings
if(WIN32)
  add_compile_definitions(BUILDING_DLL LLIST_EXPORTS)
  find_program(CMAKE_MAKE_PROGRAM NAMES ninja ninja-build make mingw32-make)
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  find_package(Threads REQUIRED)
endif()

# Warnings
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()


# Library target
add_library(llist SHARED src/llist.c)

# Modern target-based include directories
target_include_directories(llist
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

# Platform-specific linking
if(NOT WIN32)
    target_link_libraries(llist PRIVATE Threads::Threads)
endif()

# Output configuration
set_target_properties(llist PROPERTIES
    OUTPUT_NAME "llist"
    PREFIX ""
    DEBUG_POSTFIX "_d"  # For debug builds
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Install rules with better component support
install(TARGETS llist
    EXPORT llistTargets
    RUNTIME DESTINATION bin COMPONENT Runtime
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    INCLUDES DESTINATION include
)

install(FILES include/llist.h
    DESTINATION include
    COMPONENT Development
)

# Generate and install package config
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/llistConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/llistConfig.cmake"
    INSTALL_DESTINATION lib/cmake/llist
)

# Testing configuration
option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
    enable_testing()
	option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
	if(ENABLE_COVERAGE)
		if(NOT WIN32)  # Coverage typically works on Unix-like systems
			add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
			add_link_options(--coverage)
		else()
			message(WARNING "Code coverage not supported on this platform")
		endif()
	endif()
    add_subdirectory(tests)
endif()

# Export targets for find_package support
install(EXPORT llistTargets
    FILE llistTargets.cmake
    NAMESPACE llist::
    DESTINATION lib/cmake/llist
)

# Package information
set(CPACK_PACKAGE_NAME "llist")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "rb5014")
include(CPack)