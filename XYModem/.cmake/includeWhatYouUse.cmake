
find_program (
    INCLUDE_WHAT_YOU_USE_EXE
    NAMES iwyu iwyu.exe
    HINTS "/usr/bin"
    DOC "include-what-you-use executable"
    NO_CACHE
    REQUIRED
   )

set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE_EXE})
