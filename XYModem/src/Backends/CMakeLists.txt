set(BACKEND_HEADER_LIST "Devices/SerialHandler.h" "Files/DesktopFile.h"
                        "Loggers/Spdlogger.h")
set(XYModem_HEADER_LIST "Devices/DeviceHandler.h" "Files/File.h"
                        "Loggers/Logger.h")
set(BACKEND_SRC_LIST "Devices/SerialHandler.cpp" "Files/DesktopFile.cpp"
                     "Loggers/Spdlogger.cpp")
list(TRANSFORM BACKEND_HEADER_LIST
     PREPEND "${CMAKE_CURRENT_LIST_DIR}/../../include/Backends/")
list(TRANSFORM XYModem_HEADER_LIST
     PREPEND "${CMAKE_CURRENT_LIST_DIR}/../../include/XYModem/")
list(TRANSFORM BACKEND_SRC_LIST PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

# Make an automatic library - will be static or dynamic based on user setting
add_library(XYModemBackends STATIC ${BACKEND_SRC_LIST} ${BACKEND_HEADER_LIST}
                                   ${XYModem_HEADER_LIST})
add_dependencies(XYModemBackends XYModemCore)
target_link_libraries(XYModemBackends PRIVATE XYModemCore)

if(APPLE)
  # Fixes strange linker error, which seems to happen from time to time
  target_link_options(XYModemBackends PUBLIC
                      "LINKER:-U,___darwin_check_fd_set_overflow")
endif(APPLE)

# We need this directory, and users of our library will need it too
target_include_directories(
  XYModemBackends PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/Backends)
target_include_directories(
  XYModemBackends PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/XYModem)

target_link_libraries(
  XYModemBackends PUBLIC CONAN_PKG::serial CONAN_PKG::spdlog
                         CONAN_PKG::ghc-filesystem CONAN_PKG::gtest)

target_compile_options(
  XYModemBackends
  PRIVATE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:
          /MTd
          /W4
          /WX
          /Od
          >
          $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:
          /MT
          /W4
          /WX
          /O2
          >
          $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<CONFIG:Debug>>:
          -O0>
          $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<CONFIG:Release>>:
          -O3>
          $<$<CXX_COMPILER_ID:AppleClang>:
          -Werror
          -Wall
          -Wextra
          -pedantic
          -Wold-style-cast
          -Wshadow
          -Wctor-dtor-privacy>)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include/Backends"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
