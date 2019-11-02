cmake_minimum_required(VERSION 3.14)
project(astl VERSION 0.0.1 LANGUAGES CXX)

include(cmake/gtest.cmake)
include(cmake/doxygen.cmake)
set(CMAKE_CXX_STANDARD 17)

setup_doxygen()

message(STATUS " * ASTL version         ${PROJECT_VERSION}")
message(STATUS " * CXX Compiler         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " * CXX version          ${CMAKE_CXX_STANDARD}")
message(STATUS " * ASTL_GTEST           ${ASTL_GTESTS}")
message(STATUS " * Doxygen Version      ${DOXYGEN_VERSION}")

add_subdirectory(src)

if(ASTL_GTESTS)
    astl_enable_gtest()
endif()

