cmake_minimum_required(VERSION 3.10)
project(sharedlib VERSION 1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(sharedlib INTERFACE)

target_include_directories(sharedlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
