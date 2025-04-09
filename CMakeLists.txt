cmake_minimum_required(VERSION 3.10)
project(llist LANGUAGES C)

# Platform-specific settings
if(WIN32)
    add_compile_definitions(BUILDING_DLL)
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
    OUTPUT_NAME "llist"  # libllist.so on Unix, llist.dll on Windows
)

# Optional: Install rules
install(TARGETS llist DESTINATION lib)
install(FILES include/llist.h DESTINATION include)
