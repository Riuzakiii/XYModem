add_executable(example_xmodem example_xmodem.cpp CLIParser.cpp)
target_include_directories(
  example_xmodem PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem
                         ${CMAKE_CURRENT_LIST_DIR}/../include/Backends)
target_link_libraries(example_xmodem PUBLIC XYModem CONAN_PKG::utfcpp
                                            CONAN_PKG::serial CONAN_PKG::spdlog)

target_compile_options(
  example_xmodem
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
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -fprofile-arcs
    -ftest-coverage>
    $<$<CXX_COMPILER_ID:AppleClang>:
    -Werror
    -Wall
    -Wextra
    -pedantic
    -Wold-style-cast
    -Wshadow
    -Wctor-dtor-privacy>)

add_executable(example_ymodem example_ymodem.cpp CLIParser.cpp)
target_include_directories(
  example_ymodem PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem
                         ${CMAKE_CURRENT_LIST_DIR}/../include/Backends)
target_link_libraries(example_ymodem PUBLIC XYModem CONAN_PKG::utfcpp
                                            CONAN_PKG::serial CONAN_PKG::spdlog)

target_compile_options(
  example_ymodem
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
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -fprofile-arcs
    -ftest-coverage>
    $<$<CXX_COMPILER_ID:AppleClang>:
    -Werror
    -Wall
    -Wextra
    -pedantic
    -Wold-style-cast
    -Wshadow
    -Wctor-dtor-privacy>)
