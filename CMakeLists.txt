cmake_minimum_required(VERSION 3.22)
project(Bookstore VERSION 0.1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(Bookstore src/BookStorage.cpp)

# Include directories
include_directories(src/include)

# Link libraries (if any)
# target_link_libraries(Bookstore <library_name>)