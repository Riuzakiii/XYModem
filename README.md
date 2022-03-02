XYModem is a library that allows you to send files using the XModem or YModem 1K file transfer protocols. It's development started at Expressive E (https://www.expressivee.com/), which allowed me to continue this project under an open-source licence. Anyone can use this library for free under the MIT licence, don't hesitate to make contributions following the contribution guidelines.

The default implementation is made over the Serial communication protocol but it can easily be used over another protocol/serial library by implementing another backend (DeviceHandler).

# What's next

1. Reduce dependencies and add options to use without Conan.
2. Add switch between XModem/YModem 1K or 128 and YModem 1K batch/single file
3. Add XModem/YModem receiver, and complete tests by testing a transmission internally. Maybe add fuzzy testing.
4. Use latest C++ idioms

# Install

## Requirements

You must have the following softwares installed
* `cmake` : a buildchain software and language (https://cmake.org/)
* `conan` : a package manager for C/C++ (https://conan.io/)
* `python` : Conan requires python >= 3.5
* `git` : for versioning
* a C++17 compatible compiler 

The versions of the compilers and SDK (Software Development Kits, provided by the OS in general) are in the file _VERSIONS.md_.

## On Windows

1. Generate the Buildsystem `cmake -B build`. You can specify the generator using `-G` but it should be already set to _Visual Studio 16 2019_

The project files generated can be opened in Microsoft Visual Studio.

2. Build XYModem using 
   * `cmake --build build --config Debug` for the Debug build
   * `cmake --build build --config Release` for the Release build

## On MacOs

1. Generate the Buildsystem using
   * `cmake -B build -DCMAKE_BUILD_TYPE=Debug` if you want to build in Debug
   * `cmake -B build -DCMAKE_BUILD_TYPE=Release` if you want to build in Release
   * `cmake -B build -G 'Xcode'` if you want to build in Debug mode in XCode. The project files generated can be opened in XCode
2. Build XYModem using 
   * `cmake --build build --config Debug` for the Debug build
   * `cmake --build build --config Release` for the Release build

## Building the examples

Add the `-DXYMODEM_BUILD_EXAMPLES=On` flag in the first step to build the examples.


# Testing XYModem

All transitions of XModem and YModem have been tested. Testing is made using the GTest framework.
The examples have been tested using the Tera Term receiver for XModem/YModem.

To run the tests, first build XYModem then run `ctest build` to run the tests in the _build_ folder.

# Sources
The following sources could help you understand this protocol 

* https://en.wikipedia.org/wiki/YMODEM
* http://pauillac.inria.fr/~doligez/zmodem/ymodem.txt (Protocol reference)
* https://programmer.ink/think/ymodem-protocol-learning.html
* https://ttssh2.osdn.jp/manual/4/en/reference/sourcecode.html
