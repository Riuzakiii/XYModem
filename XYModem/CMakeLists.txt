cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17) # Default CPP standard.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET
    "10.12"
    CACHE STRING "Minimum OS X deployment version")

set(WITH_STATIC_ANALYSIS OFF CACHE BOOL "Runs clang-tidy on this project")
set(USES_CLANG_COMPILER OFF CACHE BOOL "Indicates if we want to compile with clang (Linux only)")

if(UNIX AND NOT APPLE AND ${USES_CLANG_COMPILER})
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/clang.cmake)
  set_clang_as_main_compiler()
endif()

set(GLOBAL_APP_NAME "XYModem") 
project(${GLOBAL_APP_NAME})

if("${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles" AND NOT DEFINED
                                                      CMAKE_BUILD_TYPE)
  message(
    FATAL_ERROR
      "The variable CMAKE_BUILD_TYPE must be defined when using the generator \"Unix Makefiles\""
  )
endif()

if("${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/${GLOBAL_APP_NAME}/${CMAKE_BUILD_TYPE})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/${GLOBAL_APP_NAME}/${CMAKE_BUILD_TYPE})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/${GLOBAL_APP_NAME}/${CMAKE_BUILD_TYPE})
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  option(XYMODEM_BUILD_EXAMPLES "Build examples" ON)
else()
  option(XYMODEM_BUILD_EXAMPLES "Build examples" OFF)
endif()

if(${WITH_STATIC_ANALYSIS})
  include (${CMAKE_CURRENT_LIST_DIR}/.cmake/clangTidy.cmake)
endif()

enable_testing()

include(${CMAKE_CURRENT_LIST_DIR}/setupDependencies.cmake)

add_subdirectory(tests)
add_subdirectory(src)
if(XYMODEM_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
