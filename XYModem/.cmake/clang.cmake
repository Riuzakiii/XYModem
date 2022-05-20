
function(set_clang_as_main_compiler)
    find_program (
        CLANG_EXE
        NAMES clang clang.exe
        HINTS "/usr/bin"
        DOC "Location of the clang executable"
        NO_CACHE
        REQUIRED
    )
    find_program (
        CLANG++_EXE
        NAMES clang++ clang++.exe
        HINTS "/usr/bin"
        DOC "Location of the clang executable"
        NO_CACHE
        REQUIRED
    )

    set(CMAKE_C_COMPILER ${CLANG_EXE} PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER ${CLANG++_EXE} PARENT_SCOPE)
endfunction()


function(get_current_clang_version output_variable)
    find_program (
        CLANG_EXE
        NAMES clang clang.exe
        HINTS "/usr/bin"
        DOC "Location of the clang executable"
        NO_CACHE
        REQUIRED
    )

    execute_process(COMMAND ${CLANG_EXE} --version
        OUTPUT_VARIABLE CLANG_VERSION_RAW_OUTPUT
    )

    # Keeps only the first line of the `clang --version` output
    string(FIND "${CLANG_VERSION_RAW_OUTPUT}" "\n" CLANG_VERSION_RAW_OUTPUT_FIRST_NEW_LINE_POSITION)
    string(SUBSTRING "${CLANG_VERSION_RAW_OUTPUT}" 0 ${CLANG_VERSION_RAW_OUTPUT_FIRST_NEW_LINE_POSITION} CLANG_VERSION_RAW_OUTPUT)

    # Removes everything before the version of the `clang --version` output (before the last space of the line)
    string(FIND "${CLANG_VERSION_RAW_OUTPUT}" " " CLANG_VERSION_RAW_OUTPUT_LAST_SPACE_POSITION REVERSE)
    string(SUBSTRING "${CLANG_VERSION_RAW_OUTPUT}" ${CLANG_VERSION_RAW_OUTPUT_LAST_SPACE_POSITION} -1 CLANG_VERSION_RAW_OUTPUT)

    # Removes everything after  the version of the `clang --version` output (after the character '-')
    string(FIND "${CLANG_VERSION_RAW_OUTPUT}" ".0" CLANG_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION REVERSE)
    if(NOT ${CLANG_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION} EQUAL -1)
        string(SUBSTRING "${CLANG_VERSION_RAW_OUTPUT}" 0 ${CLANG_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION} CLANG_VERSION_RAW_OUTPUT)
    endif()
    
    # Removes the `.0` at the end of the version, if there is one
    string(FIND "${CLANG_VERSION_RAW_OUTPUT}" ".0" CLANG_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION REVERSE)
    if(NOT ${CLANG_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION} EQUAL -1)
        string(SUBSTRING "${CLANG_VERSION_RAW_OUTPUT}" 0 ${CLANG_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION} CLANG_VERSION_RAW_OUTPUT)
    endif()

    string(STRIP "${CLANG_VERSION_RAW_OUTPUT}" CLANG_VERSION_RAW_OUTPUT)
    set (${output_variable} ${CLANG_VERSION_RAW_OUTPUT} PARENT_SCOPE)
endfunction()
