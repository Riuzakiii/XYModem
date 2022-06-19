#!/bin/sh

clang-format --verbose -i XYModem/src/*/*.cpp
clang-format --verbose -i XYModem/src/*/*/*.cpp XYModem/src/*/*/*.hpp
clang-format --verbose -i XYModem/include/*/*.h
clang-format --verbose -i XYModem/include/*/*/*.h
clang-format --verbose -i XYModem/examples/*.cpp XYModem/examples/*.h
clang-format --verbose -i XYModem/tests/*.cpp

echo "Formatting the cmake scripts"
cmake-format -i XYModem/*.cmake XYModem/CMakeLists.txt
cmake-format -i XYModem/*/CMakeLists.txt
cmake-format -i XYModem/*/*/CMakeLists.txt
