cmake_minimum_required(VERSION 3.15)
project(hello CXX)

find_package(libnl REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} libnl::libnl)
