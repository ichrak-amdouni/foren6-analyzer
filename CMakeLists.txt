cmake_minimum_required(VERSION 2.8)
project(rplanalyzer)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-Wall -std=gnu99)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../dist/Debug/GNU-Linux-x86/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../dist/Debug/GNU-Linux-x86/)

add_subdirectory(src)

file(GLOB ANALYZER_CMAKE_MODULE cmake-modules/*.cmake)
install(FILES ${ANALYZER_CMAKE_MODULE} DESTINATION lib/rplanalyzer)
