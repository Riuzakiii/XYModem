
function(get_current_gcc_version output_variable)
    find_program (
        GCC_EXE
        NAMES gcc gcc.exe
        HINTS "/usr/bin"
        DOC "Location of the gcc exxecutable"
        NO_CACHE
        REQUIRED
    )

    execute_process(COMMAND ${GCC_EXE} --version
        OUTPUT_VARIABLE GCC_VERSION_RAW_OUTPUT
    )

    # Keeps only the first line of the `gcc --version` output
    string(FIND "${GCC_VERSION_RAW_OUTPUT}" "\n" GCC_VERSION_RAW_OUTPUT_FIRST_NEW_LINE_POSITION)
    string(SUBSTRING "${GCC_VERSION_RAW_OUTPUT}" 0 ${GCC_VERSION_RAW_OUTPUT_FIRST_NEW_LINE_POSITION} GCC_VERSION_RAW_OUTPUT)

    # Keeps only the actual version of the `gcc --version` output (after the last space of the line)
    string(FIND "${GCC_VERSION_RAW_OUTPUT}" " " GCC_VERSION_RAW_OUTPUT_LAST_SPACE_POSITION REVERSE)
    string(SUBSTRING "${GCC_VERSION_RAW_OUTPUT}" ${GCC_VERSION_RAW_OUTPUT_LAST_SPACE_POSITION} -1 GCC_VERSION_RAW_OUTPUT)

    # Removes the `.0` at the end of the version, if there is one
    string(FIND "${GCC_VERSION_RAW_OUTPUT}" ".0" GCC_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION REVERSE)
    if(NOT ${GCC_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION} EQUAL -1)
        string(SUBSTRING "${GCC_VERSION_RAW_OUTPUT}" 0 ${GCC_VERSION_RAW_OUTPUT_DOT_ZERO_POSITION} GCC_VERSION_RAW_OUTPUT)
    endif()

    string(STRIP "${GCC_VERSION_RAW_OUTPUT}" GCC_VERSION_RAW_OUTPUT)
    set (${output_variable} ${GCC_VERSION_RAW_OUTPUT} PARENT_SCOPE)
endfunction()
