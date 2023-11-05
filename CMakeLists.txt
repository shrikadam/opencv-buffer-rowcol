cmake_minimum_required(VERSION 3.16)

project(line_stitching)

find_package(OpenCV REQUIRED)

add_executable(stitch_lines main.cpp)

target_link_libraries(stitch_lines ${OpenCV_LIBS} )
