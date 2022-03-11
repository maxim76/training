cmake_minimum_required (VERSION 3.15)

# Create solution with this name (not project but solution)
project (libzmq_test)
# Enable c++17
set(CMAKE_CXX_STANDARD 17)

set(ZMQLIB_DIR CACHE STRING "libzmq static library directory")
if(NOT ZMQLIB_DIR)
    message(FATAL_ERROR "Please, specify libzmq directory via -DZMQLIB_DIR=<dir>")
endif()
message(STATUS "libzmq dir is ${ZMQLIB_DIR}, change via -DZMQLIB_DIR=<dir>")

# Enable directories
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build result will be written to
set(EXECUTABLE_OUTPUT_PATH "../bin/windows")

# Add -Wall -Werror flags
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX-)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_definitions(-DZMQ_STATIC)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

# Define projects/executables
add_executable(client	client.cpp)
add_executable(server	server.cpp)
add_executable(version	version.cpp)

target_include_directories(client PUBLIC "${ZMQLIB_DIR}/include")
target_include_directories(server PUBLIC "${ZMQLIB_DIR}/include")
target_include_directories(version PUBLIC "${ZMQLIB_DIR}/include")

target_link_libraries(client "${ZMQLIB_DIR}/lib/libzmq-mt-s-4_3_4.lib" ${SYSTEM_LIBS} Ws2_32.lib Iphlpapi.lib)
target_link_libraries(server "${ZMQLIB_DIR}/lib/libzmq-mt-s-4_3_4.lib" ${SYSTEM_LIBS} Ws2_32.lib Iphlpapi.lib)
target_link_libraries(version "${ZMQLIB_DIR}/lib/libzmq-mt-s-4_3_4.lib" ${SYSTEM_LIBS} Ws2_32.lib Iphlpapi.lib)
