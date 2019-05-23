cmake_minimum_required (VERSION 3.0)

# Enable directories
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create solution with this name (not project but solution)
project (skiena)

# Build result will be written to
set(EXECUTABLE_OUTPUT_PATH "../bin/windows")

# Define projects/executables
add_executable(ex01_01 ex01_01.cpp)
add_executable(ex07_01 ex07_01.cpp)

# Group targets (executables) into corresponding chapters
SET_PROPERTY( TARGET ex01_01 PROPERTY FOLDER "Chapter01" )
SET_PROPERTY( TARGET ex07_01 PROPERTY FOLDER "Chapter07" )
