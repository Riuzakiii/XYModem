cmake_minimum_required(VERSION 3.13.0)

if(APPLE)
  execute_process(
    COMMAND conan install . --install-folder=build -pr=.conan/osx_profile
            --build=missing -s build_type=Release
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND conan install . --install-folder=build -pr=.conan/osx_profile
            --build=missing -s build_type=Debug
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
else()
  execute_process(
    COMMAND conan install . --install-folder=build -pr=.conan/windows_profile
            --build=missing -s build_type=Debug -s compiler.runtime=MTd
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND conan install . --install-folder=build -pr=.conan/windows_profile
            --build=missing -s build_type=Release -s compiler.runtime=MT
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)
