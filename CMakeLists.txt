cmake_minimum_required(VERSION 3.20)
set(CMAKE_DEPENDS_USE_COMPILER OFF)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_C cmake/tigcc_override.cmake)
project(tmp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "Building within the source tree is unsupported")
endif()

if(NOT DEFINED ENV{TIGCC})
  message(FATAL_ERROR "Please set the environment TIGCC to point to the TIGCC installation")
endif()

add_subdirectory(src)
