cmake_minimum_required(VERSION 3.20)
set(CMAKE_DEPENDS_USE_COMPILER OFF)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_C cmake/tigcc_override.cmake)
project(tmp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections --optimize-code --cut-ranges --reorder-sections --merge-constants --remove-unused -Wall -Wno-missing-field-initializers -W -Wwrite-strings")

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "Building within the source tree is unsupported")
endif()

if(NOT DEFINED ENV{TIGCC})
  message(FATAL_ERROR "Please set the environment TIGCC to point to the TIGCC installation")
endif()

option(BUILD_GAME "Build meow game" ON)

add_subdirectory(lib)

if(BUILD_GAME)
  add_subdirectory(src)
endif()
