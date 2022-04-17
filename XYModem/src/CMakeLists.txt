set(HEADER_LIST "XYModem.h" "XYModemConst.h" "Senders/YModemSender.h" "Senders/XModemSender.h" "Devices/DeviceHandler.h"
                "Devices/SerialHandler.h" "FileTransferProtocol.h" "Files/File.h" "Files/DesktopFile.h" "Loggers/Logger.h" "Loggers/Spdlogger.h")
set(SRC_LIST "Devices/DeviceHandler.cpp" "Devices/SerialHandler.cpp" "Files/File.cpp" "Files/DesktopFile.cpp" "Loggers/Logger.cpp" "Loggers/Spdlogger.cpp" "FileTransferProtocol.cpp" "Senders/YModemSender.cpp" "Senders/XModemSender.cpp")
list(TRANSFORM HEADER_LIST
     PREPEND "${CMAKE_CURRENT_LIST_DIR}/../include/XYModem/")
list(TRANSFORM SRC_LIST PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

# Make an automatic library - will be static or dynamic based on user setting
add_library(XYModem STATIC ${SRC_LIST} ${HEADER_LIST})

if(APPLE)
  # Fixes strange linker error, which seems to happen from time to time
  target_link_options(XYModem PUBLIC
                      "LINKER:-U,___darwin_check_fd_set_overflow")
endif(APPLE)

# We need this directory, and users of our library will need it too
target_include_directories(XYModem
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem)

target_link_libraries(XYModem PUBLIC CONAN_PKG::serial CONAN_PKG::spdlog CONAN_PKG::ghc-filesystem CONAN_PKG::gtest)

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
