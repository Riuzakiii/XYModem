add_executable(example_xmodem example_xmodem.cpp CLIParser.cpp)
target_include_directories(example_xmodem
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem)
target_link_libraries(example_xmodem XYModem)

if(NOT spdlog_FOUND)
  find_package("spdlog")
endif()
if(TARGET spdlog::spdlog)
  target_link_libraries(example_xmodem spdlog::spdlog)
endif()

if(NOT utf8cpp_FOUND)
  find_package("utf8cpp")
endif()
if(TARGET utf8cpp::utf8cpp)
  target_link_libraries(example_xmodem utf8cpp::utf8cpp)
endif()

target_compile_options(
  example_xmodem
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

add_executable(example_ymodem example_ymodem.cpp CLIParser.cpp)
target_include_directories(example_ymodem
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include/XYModem)
target_link_libraries(example_ymodem XYModem)

if(NOT spdlog_FOUND)
  find_package("spdlog")
endif()
if(TARGET spdlog::spdlog)
  target_link_libraries(example_ymodem spdlog::spdlog)
endif()

if(NOT utf8cpp_FOUND)
  find_package("utf8cpp")
endif()
if(TARGET utf8cpp::utf8cpp)
  target_link_libraries(example_ymodem utf8cpp::utf8cpp)
endif()

target_compile_options(
  example_ymodem
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
