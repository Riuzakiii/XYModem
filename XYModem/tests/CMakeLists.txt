file(GLOB TEST_FILES *.cpp *.h)

add_executable(unitTest MACOSX_BUNDLE ${TEST_FILES})

target_include_directories(unitTest
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem)
target_link_libraries(unitTest PUBLIC XYModem CONAN_PKG::crc_cpp)

target_compile_options(
  unitTest
  PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:
    /MTd
    /W4
    /WX
    /Od>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:
    /MT
    /W4
    /WX
    /O2>
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

include(GoogleTest)
if(NOT APPLE OR "${CMAKE_OSX_ARCHITECTURES}" STREQUAL
                "${CMAKE_SYSTEM_PROCESSOR}")
  gtest_discover_tests(unitTest)
endif()
