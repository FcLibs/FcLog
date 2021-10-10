cmake_minimum_required(VERSION 3.0)
project(FcLog C)

set(CMAKE_C_STANDARD 11)

file(GLOB PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/src/*.h")

add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE PROJECT_SOURCES)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/)

