[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)

<p align = "center"><img src="XYModem.svg" style = "width : 60%;"/></p>


**XYModem** is a library that allows you to send files using the XModem or YModem file transfer protocols. It is designed to be easily portable to other platforms by simply implementing new backends, keeping the core functionnalities independent.

# Platforms

| Platform | Status |
|:--------:|--------|
|Windows Visual Studio 2022| [![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20windows_2022_release)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|Windows Visual Studio 2019| [![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20windows_2019_release)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|macOS 10.15 XCode 12.4 |[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20mac_10_15_release_xcode_12_4)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|macOS 11 XCode 13.2.1 |[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20mac_11_release_xcode_13_2_1)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|macOS 12 XCode 13.4 ARM64 |[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20mac_12_arm64_release_xcode_13_4)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|Ubuntu 18.04 GCC10|[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20ubuntu_18_04_release_gcc_10)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|Ubuntu 20.04 GCC10|[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20ubuntu_20_04_release_gcc_10)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|Ubuntu 22.04 GCC10|[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20ubuntu_22_04_release_gcc_10)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|Ubuntu 22.04 GCC11|[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=XYModem&configuration=XYModem%20ubuntu_22_04_release_gcc_11)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|
|Static analysis|[![Build Status](https://dev.azure.com/Riuzakiii/XYModem/_apis/build/status/Riuzakiii.XYModem?branchName=main&jobName=Static_Analysis&configuration=Static_Analysis%20release)](https://dev.azure.com/Riuzakiii/XYModem/_build/latest?definitionId=1&branchName=main)|

# Add XYModem to your project

## Requirements

You must have the following softwares installed
* `cmake` : a buildchain software and language (https://cmake.org/)
* a C++17 compatible compiler 

Additionnal requirements are needed for build the backends, the examples and testing
* `conan` : a package manager for C/C++ (https://conan.io/)
* `python` : Conan requires python >= 3.5

## Add XYModem to your project

Add these lines to your `CMakeLists.txt`

1. `add_subdirectory(XYModemFolder/XYModem)`
2. `target_link_libraries(target XYModem)`

By default, only the core functionnalities are built. To add the backends, examples or testing, add this to the CMake build command : 

`-DXYMODEM_BUILD_EXAMPLES`, `-DXYMODEM_BUILD_BACKENDS`, `-DXYMODEM_BUILD_TESTS` or `-DXYMODEM_BUILD_ALL`.
## Include the right headers

1. XYModem uses abstract classes for loggers, files and communication devices. You can use the implementations already provided for desktop OS (`SerialHandler.h`, `DesktopFile.h`, `Spdlogger.h`), or implement your own for other platforms (embedded for instance). The existing backends can be found in _include/Backends_.
2. Include one of the senders in _include/XYModem/Senders_, and give it the required instances of DeviceHandler, File and Logger. Logging is optionnal (default initialization), but File and DeviceHandler are needed to communicate.

On a desktop platform, you can include `XYModem/XYModem.h`, which includes the main headers and the default implementations of DeviceHandler, File, and Logger.

# Testing

All transitions of XModem and YModem are tested in the build pipeline. Testing is made using the GTest framework.
The examples have been manually tested using the Tera Term receiver for XModem/YModem on Windows.

# Acknowledgment 

Thanks to Expressive E (https://www.expressivee.com/), which allowed me to continue this project under an open-source licence.

## Contributors

<table>
<thead>
  <tr>
    <td><div style = "text-align:center;"><a href = "https://github.com/Riuzakiii"><img src="https://avatars.githubusercontent.com/u/66674723" style ="width:100px;height:100px;"/> </br> @Riuzakiii </a></div></td>
    <td><div style = "text-align:center;"><a href = "https://github.com/Xav83"><img src="https://avatars.githubusercontent.com/u/6787157?v=4" style ="width:100px;height:100px;"/> <br/>  @Xav83</a></div></td>
  </tr>
</thead>
</table>

# Sources
The following sources could help you understand this protocol 

* https://en.wikipedia.org/wiki/YMODEM
* http://pauillac.inria.fr/~doligez/zmodem/ymodem.txt (Protocol reference)
* https://programmer.ink/think/ymodem-protocol-learning.html
* https://ttssh2.osdn.jp/manual/4/en/reference/sourcecode.html
* http://www.ross.net/crc/download/crc_v3.txt
