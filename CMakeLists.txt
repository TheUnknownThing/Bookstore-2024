cmake_minimum_required(VERSION 3.22)
project(Bookstore VERSION 0.1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

option(DEBUG_MODE "Build for debugging" OFF)
if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE)
endif()

# Add the executable
add_executable(code src/Bookstore.cpp)

# Include directories
include_directories(src/include)