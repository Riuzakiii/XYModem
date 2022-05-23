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
  include(${CMAKE_CURRENT_LIST_DIR}/.cmake/xcode.cmake)
  get_xcode_version_for_apple_clang(APPLE_CLANG_VERSION)
  get_conan_architecture_for_osx_build_architecture(CONAN_ARCHITECTURE_NAME)
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/osx_profile
            --build=missing -s build_type=Release -s arch=${CONAN_ARCHITECTURE_NAME} -s arch_build=${CONAN_ARCHITECTURE_NAME} -s compiler.version=${APPLE_CLANG_VERSION}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/osx_profile
            --build=missing -s build_type=Debug -s arch=${CONAN_ARCHITECTURE_NAME} -s arch_build=${CONAN_ARCHITECTURE_NAME} -s compiler.version=${APPLE_CLANG_VERSION}
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
  if(${USES_CLANG_COMPILER})
    set(CONAN_PROFILE linux_clang_profile)
    get_conan_compiler_version_for_clang(COMPILER_VERSION)
  else()
    set(CONAN_PROFILE linux_gcc_profile)

    include(${CMAKE_CURRENT_LIST_DIR}/.cmake/gcc.cmake)
    get_current_gcc_version(COMPILER_VERSION)
  endif()

  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/${CONAN_PROFILE}
            --build=missing -s build_type=Debug -s compiler.version=${COMPILER_VERSION}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(
    COMMAND ${CONAN_EXE} install . --install-folder=build -pr=${CMAKE_CURRENT_LIST_DIR}/.conan/${CONAN_PROFILE}
            --build=missing -s build_type=Release -s compiler.version=${COMPILER_VERSION}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)
