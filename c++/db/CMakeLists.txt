cmake_minimum_required (VERSION 3.0)

# Enable directories
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create solution with this name (not project but solution)
project (db)

# Build result will be written to
set(EXECUTABLE_OUTPUT_PATH "../../bins/db/windows")

# Define additional library directories
link_directories( BEFORE third_party/sqlite )

# Define projects/executables
add_executable(sqlite_demo sqlite_demo.cpp)

# Group targets (executables) into corresponding chapters
#SET_PROPERTY( TARGET sqlite_demo PROPERTY FOLDER "Chapter01" )
