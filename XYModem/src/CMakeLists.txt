add_subdirectory(XYModem/)
add_library(XYModem INTERFACE)

if(${BUILD_EXAMPLES}
   OR ${BUILD_BACKEND}
   OR ${BUILD_TESTS})
  add_subdirectory(Backends/)
  target_link_libraries(
    XYModem
    INTERFACE
      XYModemCore
      XYModemBackends
      $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
      -fprofile-arcs
      -ftest-coverage>)
else()
  target_link_libraries(
    XYModem
    INTERFACE
      XYModemCore
      $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
      -fprofile-arcs
      -ftest-coverage>)
endif()

target_compile_options(
  XYModem
  INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -fprofile-arcs
    -ftest-coverage>)
