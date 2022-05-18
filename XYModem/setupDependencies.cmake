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
execute_process(
  COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/windows_profile
          --build=missing -s build_type=Debug -s compiler.runtime=MTd
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
execute_process(
  COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/windows_profile
          --build=missing -s build_type=Release -s compiler.runtime=MT
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
else()
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/linux_profile
            --build=missing -s build_type=Debug
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/linux_profile
            --build=missing -s build_type=Release
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)
