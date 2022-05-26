
find_program (
    CLANG_TIDY_EXE
    NAMES clang-tidy clang-tidy.exe
    HINTS "/usr/bin"
    DOC "clang-tidy executable"
    NO_CACHE
    REQUIRED
   )

set(CLANG_TIDY_MODERNIZE_CHECKS modernize-*,-modernize-avoid-c-arrays,-modernize-use-trailing-return-type)
set(CLANG_TIDY_CHECKS bugprone-*,clang-analyzer-*,concurrency-*,${CLANG_TIDY_MODERNIZE_CHECKS},performance-*,portability-*)
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} --checks=${CLANG_TIDY_CHECKS} --warnings-as-errors=${CLANG_TIDY_CHECKS})
