
find_program (
    CPPLINT_EXE
    NAMES cpplint cpplint.exe
    HINTS "/usr/bin"
    DOC "cpplint executable"
    NO_CACHE
    REQUIRED
   )

set(CMAKE_CXX_CPPLINT ${CPPLINT_EXE})
