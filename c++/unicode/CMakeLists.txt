cmake_minimum_required (VERSION 3.0)

# Create solution with this name (not project but solution)
project (unicode)

# Enable c++17
set(CMAKE_CXX_STANDARD 17)

# Build result will be written to
set(EXECUTABLE_OUTPUT_PATH "../../bins/unicode/windows")

# Define projects/executables
add_executable(unicode unicode.cpp)

