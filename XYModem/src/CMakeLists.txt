set(HEADER_LIST "XYModem.h" "XYModemConst.h" "Devices/DeviceHandler.h"
                "Devices/SerialHandler.h" "FileTransferProtocol.h")
set(SRC_LIST "XYModem.cpp" "FileTransferProtocol.cpp"
             "Devices/DeviceHandler.cpp" "Devices/SerialHandler.cpp")
list(TRANSFORM HEADER_LIST
     PREPEND "${CMAKE_CURRENT_LIST_DIR}/../include/XYModem/")
list(TRANSFORM SRC_LIST PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

# Make an automatic library - will be static or dynamic based on user setting
add_library(XYModem ${SRC_LIST} ${HEADER_LIST})

if(APPLE)
  # Fixes strange linker error, which seems to happen from time to time
  target_link_options(XYModem PUBLIC
                      "LINKER:-U,___darwin_check_fd_set_overflow")
endif(APPLE)

# We need this directory, and users of our library will need it too
target_include_directories(XYModem
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem)

if(NOT GTest_FOUND)
  find_package("GTest")
endif()
if(NOT spdlog_FOUND)
  find_package("spdlog")
endif()
if(NOT benchmark_FOUND)
  find_package("benchmark")
endif()
if(NOT serial_FOUND)
  find_package("serial")
endif()
if(NOT crc_cpp_FOUND)
  find_package("crc_cpp")
endif()
if(NOT ghcFilesystem_FOUND)
  find_package("ghcFilesystem")
endif()

if(TARGET serial::serial)
  target_link_libraries(XYModem serial::serial)
endif()
if(TARGET crc_cpp::crc_cpp)
  target_link_libraries(XYModem crc_cpp::crc_cpp)
endif()
if(TARGET spdlog::spdlog)
  target_link_libraries(XYModem spdlog::spdlog)
endif()
if(TARGET ghcFilesystem::ghcFilesystem)
  target_link_libraries(XYModem ghcFilesystem::ghcFilesystem)
endif()
if(TARGET GTest::GTest)
  include_directories(${SPDLOG_INCLUDE_DIRS})
  target_link_libraries(XYModem GTest::GTest)
endif()

target_compile_options(
  XYModem
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
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
