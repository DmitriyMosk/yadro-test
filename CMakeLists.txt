# CMAKE VERSION
cmake_minimum_required(VERSION 3.10)

# PROJECT NAME
project(DevProject) 

# C++ Standart
set(CMAKE_CXX_STANDARD 20) 

include_directories(include) 

file(GLOB SOURCES "src/*.cpp")

add_executable(DevProject ${SOURCES}) 

enable_testing()
add_subdirectory(tests) 