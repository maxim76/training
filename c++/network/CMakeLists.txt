cmake_minimum_required (VERSION 3.0)

# Enable directories
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create solution with this name (not project but solution)
project (network)

# Build result will be written to
set(EXECUTABLE_OUTPUT_PATH "../../bins/network/windows")

# Define projects/executables
add_executable(test_udp_request test_udp_request.cpp udp_request.cpp)

