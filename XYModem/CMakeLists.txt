cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17) # Default CPP standard.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
  if("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "x86_64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET
        "10.12"
        CACHE STRING "Minimum OS X deployment version")
  elseif("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET
        "12.0"
        CACHE STRING "Minimum OS X deployment version")
  else()
    message(
      FATAL_ERROR
        "Error: Unknown architecture ! - CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}"
    )
  endif()
endif()

set(WITH_STATIC_ANALYSIS
    OFF
    CACHE BOOL "Runs clang-tidy on this project")
set(USES_CLANG_COMPILER
    OFF
    CACHE BOOL "Indicates if we want to compile with clang (Linux only)")

# Modular build, set which part you need (Core is always built)
set(BUILD_BACKEND
    OFF
    CACHE BOOL "Enable backend (requires Conan)")
set(BUILD_TESTS
    OFF
    CACHE BOOL "Enable testing (requires Conan)")
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(BUILD_EXAMPLES
      ON
      CACHE BOOL "Build examples")
  set(BUILD_BACKEND ON)
else()
  set(BUILD_EXAMPLES
      OFF
      CACHE BOOL "Build examples")
endif()
set(BUILD_ALL
    OFF
    CACHE BOOL "Build Core, Backends, Examples and Tests")
if(BUILD_ALL)
  set(BUILD_BACKEND ON)
  set(BUILD_EXAMPLES ON)
  set(BUILD_TESTS ON)
endif()

if(UNIX
   AND NOT APPLE
   AND ${USES_CLANG_COMPILER})
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

if(${BUILD_TESTS}
   OR ${BUILD_BACKEND}
   OR ${BUILD_EXAMPLES})
  include(${CMAKE_CURRENT_LIST_DIR}/setupDependencies.cmake)
endif()

if(${WITH_STATIC_ANALYSIS})
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/clangTidy.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/cppcheck.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/includeWhatYouUse.cmake)
endif()
add_subdirectory(src)

if(${BUILD_TESTS})
  enable_testing()
  add_subdirectory(tests)
endif()

if(${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()
