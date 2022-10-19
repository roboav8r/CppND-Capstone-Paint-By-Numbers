cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(PaintByNumbers)

find_package (OpenCV REQUIRED)
include_directories ("/usr/include/opencv4/")

add_executable(PaintByNumbers src/main.cpp)

target_link_libraries(PaintByNumbers ${OpenCV_LIBS} )