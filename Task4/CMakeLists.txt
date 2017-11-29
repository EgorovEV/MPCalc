cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES tsp_problem.c graph.h graph.c evolution.h evolution.c)
add_executable(tsp_problem ${SOURCE_FILES})
