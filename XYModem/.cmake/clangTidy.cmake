
find_program (
    CLANG_TIDY_EXE
    NAMES clang-tidy clang-tidy.exe
    HINTS "/usr/bin"
    DOC "clang-tidy executable"
    NO_CACHE
    REQUIRED
   )

set(CLANG_TIDY_CHECKS_READABILITY readability-*,-readability-magic-numbers)
set(CLANG_TIDY_CHECKS_CPPCORE_GUIDELINE cppcoreguidelines-*,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-avoid-c-arrays)
set(CLANG_TIDY_MODERNIZE_CHECKS modernize-*,-modernize-avoid-c-arrays,-modernize-use-trailing-return-type)
set(CLANG_TIDY_CHECKS bugprone-*,-bugprone-easily-swappable-parameters,clang-analyzer-*,concurrency-*,${CLANG_TIDY_CHECKS_CPPCORE_GUIDELINE},misc-*,${CLANG_TIDY_MODERNIZE_CHECKS},performance-*,portability-*,${CLANG_TIDY_CHECKS_READABILITY})
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} --checks=${CLANG_TIDY_CHECKS} --warnings-as-errors=${CLANG_TIDY_CHECKS})
