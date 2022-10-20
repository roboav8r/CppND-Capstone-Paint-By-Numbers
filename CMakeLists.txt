cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(PaintByNumbers)

find_package (OpenCV 4.2 EXACT REQUIRED)
include_directories ("/usr/include/opencv4/")

add_executable(PaintByNumbers src/main.cpp)
# add_executable(Dnn src/dnn.cpp)

target_link_libraries(PaintByNumbers ${OpenCV_LIBS} )
# target_link_libraries(Dnn ${OpenCV_LIBS} )