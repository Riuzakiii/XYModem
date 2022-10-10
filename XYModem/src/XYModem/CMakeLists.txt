set(HEADER_LIST
    "XYModem.h"
    "XYModemConst.h"
    "Senders/YModemSender.h"
    "Senders/XModemSender.h"
    "Receivers/XModemReceiver.h"
    "Devices/DeviceHandler.h"
    "FileTransferProtocol.h"
    "Files/File.h"
    "Loggers/Logger.h")
set(TEMPLATE_DEFINITIONS "Senders/XModemSender.hpp" "Senders/YModemSender.hpp")
set(SRC_LIST "Devices/DeviceHandler.cpp" "Files/File.cpp" "Loggers/Logger.cpp"
             "FileTransferProtocol.cpp" "Receivers/XModemReceiver.cpp")
list(TRANSFORM HEADER_LIST
     PREPEND "${CMAKE_CURRENT_LIST_DIR}/../../include/XYModem/")
list(TRANSFORM TEMPLATE_DEFINITIONS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
list(TRANSFORM SRC_LIST PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

# Make an automatic library - will be static or dynamic based on user setting
add_library(XYModemCore STATIC ${SRC_LIST} ${HEADER_LIST}
                               ${TEMPLATE_DEFINITIONS})

if(APPLE)
  # Fixes strange linker error, which seems to happen from time to time
  target_link_options(XYModemCore PUBLIC
                      "LINKER:-U,___darwin_check_fd_set_overflow")
endif(APPLE)

# We need this directory, and users of our library will need it too
target_include_directories(
  XYModemCore PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/XYModem)

target_compile_options(
  XYModemCore
  PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:
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
    -Wctor-dtor-privacy>
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -fprofile-arcs
    -ftest-coverage>)

if(${BUILD_TESTS})
  target_compile_definitions(XYModemCore PUBLIC TESTING_ENABLED=1)
  target_link_libraries(
    XYModemCore
    PUBLIC
      CONAN_PKG::gtest
      $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
      -fprofile-arcs
      -ftest-coverage>)
endif()

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include/XYModem"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
