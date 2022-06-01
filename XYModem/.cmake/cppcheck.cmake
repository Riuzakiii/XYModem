
find_program (
    CPPCHECK_EXE
    NAMES cppcheck cppcheck.exe
    HINTS "/usr/bin"
    DOC "cppcheck executable"
    NO_CACHE
    REQUIRED
   )

set(CMAKE_CXX_CPPCHECK ${CPPCHECK_EXE} --std=c++${CMAKE_CXX_STANDARD} --library=/usr/lib/x86_64-linux-gnu/cppcheck/cfg/googletest.cfg --enable=warning,style,performance --error-exitcode=1)
