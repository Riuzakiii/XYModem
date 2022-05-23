
find_program (
    CLANG_TIDY_EXE
    NAMES clang-tidy clang-tidy.exe
    HINTS "/usr/bin"
    DOC "clang-tidy executable"
    NO_CACHE
    REQUIRED
   )

set(CLANG_TIDY_CHECKS_READABILITY readability-avoid-const-params-in-decls,readability-braces-around-statements,readability-const-return-type,readability-container-contains,readability-container-data-pointer,readability-container-size-empty,readability-convert-member-functions-to-static,readability-delete-null-pointer,readability-duplicate-include,readability-else-after-return,readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-identifier-naming,readability-implicit-bool-conversion,readability-inconsistent-declaration-parameter-name,readability-isolate-declaration,readability-make-member-function-const,readability-misleading-indentation,readability-misplaced-array-index,readability-named-parameter,readability-non-const-parameter,readability-qualified-auto,readability-redundant-access-specifiers,readability-redundant-control-flow,readability-redundant-declaration,readability-redundant-function-ptr-dereference,readability-redundant-member-init)
set(CLANG_TIDY_CHECKS_CPPCORE_GUIDELINE cppcoreguidelines-*,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-avoid-c-arrays)
set(CLANG_TIDY_MODERNIZE_CHECKS modernize-*,-modernize-avoid-c-arrays,-modernize-use-trailing-return-type)
set(CLANG_TIDY_CHECKS bugprone-*,clang-analyzer-*,concurrency-*,${CLANG_TIDY_CHECKS_CPPCORE_GUIDELINE},${CLANG_TIDY_MODERNIZE_CHECKS},performance-*,portability-*,${CLANG_TIDY_CHECKS_READABILITY})
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} --checks=${CLANG_TIDY_CHECKS} --warnings-as-errors=${CLANG_TIDY_CHECKS})
