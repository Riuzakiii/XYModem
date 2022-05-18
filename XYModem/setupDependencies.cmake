cmake_minimum_required(VERSION 3.13.0)

find_program (
          CONAN_EXE
          NAMES conan conan.exe
          HINTS "~/.local/bin" "/Library/Frameworks/Python.framework/Versions/2.7/bin"
          DOC "conan executable"
          NO_CACHE
          REQUIRED
         )

if(APPLE)
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/osx_profile
            --build=missing -s build_type=Release
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/osx_profile
            --build=missing -s build_type=Debug
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
elseif(WIN32)
  if("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 16 2019")
    set(VISUAL_STUDIO_COMPILER_VERSION 16)
  elseif("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 17 2022")
    set(VISUAL_STUDIO_COMPILER_VERSION 17)
  else()
    message(FATAL_ERROR "Error: Generator not supported on Windows! - CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
  endif()

  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/windows_profile
            --build=missing -s build_type=Debug -s compiler.version=${VISUAL_STUDIO_COMPILER_VERSION} -s compiler.runtime=MTd
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/windows_profile
            --build=missing -s build_type=Release -s compiler.version=${VISUAL_STUDIO_COMPILER_VERSION} -s compiler.runtime=MT
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
else()
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/gcc.cmake)
  get_current_gcc_version(GCC_VERSION)
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/linux_profile
            --build=missing -s build_type=Debug -s compiler.version=${GCC_VERSION}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/linux_profile
            --build=missing -s build_type=Release -s compiler.version=${GCC_VERSION}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)
