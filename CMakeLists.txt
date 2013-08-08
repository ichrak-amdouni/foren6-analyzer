cmake_minimum_required(VERSION 2.8)
project(analyzer)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-Wall -std=gnu99)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../dist/Debug/GNU-Linux-x86/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../dist/Debug/GNU-Linux-x86/)

file(GLOB_RECURSE ANALYZER_SRC_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
file(GLOB_RECURSE ANALYZER_SRC_H RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

add_library(${PROJECT_NAME} SHARED ${ANALYZER_SRC_C})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
target_link_libraries(${PROJECT_NAME} pcap expat pthread)
install(TARGETS ${PROJECT_NAME} DESTINATION ./)
