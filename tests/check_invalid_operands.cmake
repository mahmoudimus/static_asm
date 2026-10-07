if(NOT DEFINED COMPILER OR NOT DEFINED SOURCE OR NOT DEFINED INCLUDE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "Missing invalid-operand compile-check arguments")
endif()

execute_process(
    COMMAND "${COMPILER}" -std=c++20 -O2 "-I${INCLUDE_DIR}" -DSTATIC_ASM_INVALID_CASE=0
        -c "${SOURCE}" -o "${OUTPUT_DIR}/invalid_operand_control.o"
    RESULT_VARIABLE control_result
    OUTPUT_VARIABLE control_stdout
    ERROR_VARIABLE control_stderr
)
if(NOT control_result EQUAL 0)
    message(FATAL_ERROR "Valid operand control failed to compile:\n${control_stdout}${control_stderr}")
endif()

execute_process(
    COMMAND "${COMPILER}" -std=c++20 -O2 -fno-exceptions "-I${INCLUDE_DIR}"
        -DSTATIC_ASM_INVALID_CASE=0 -c "${SOURCE}"
        -o "${OUTPUT_DIR}/invalid_operand_no_exceptions_control.o"
    RESULT_VARIABLE no_exceptions_result
    OUTPUT_VARIABLE no_exceptions_stdout
    ERROR_VARIABLE no_exceptions_stderr
)
if(NOT no_exceptions_result EQUAL 0)
    message(FATAL_ERROR "Valid operand control failed with exceptions disabled:\n${no_exceptions_stdout}${no_exceptions_stderr}")
endif()

foreach(case_number RANGE 1 18)
    execute_process(
        COMMAND "${COMPILER}" -std=c++20 -O2 "-I${INCLUDE_DIR}"
            "-DSTATIC_ASM_INVALID_CASE=${case_number}"
            -c "${SOURCE}" -o "${OUTPUT_DIR}/invalid_operand_${case_number}.o"
        RESULT_VARIABLE invalid_result
        OUTPUT_VARIABLE invalid_stdout
        ERROR_VARIABLE invalid_stderr
    )
    if(invalid_result EQUAL 0)
        message(FATAL_ERROR "Invalid operand case ${case_number} compiled successfully")
    endif()
    if(case_number EQUAL 1)
        set(expected_diagnostic "RIP-relative addressing cannot use an index register")
    elseif(case_number EQUAL 2 OR case_number EQUAL 3 OR case_number EQUAL 13)
        set(expected_diagnostic "high-byte registers")
    elseif(case_number EQUAL 4 OR case_number EQUAL 5)
        set(expected_diagnostic "XCHG register and memory widths must match")
    elseif(case_number EQUAL 6)
        set(expected_diagnostic "lea")
    elseif(case_number EQUAL 12)
        set(expected_diagnostic "XCHG operand widths must match")
    elseif(case_number EQUAL 14)
        set(expected_diagnostic "x86 address displacement must fit in signed 32 bits")
    elseif(case_number EQUAL 15)
        set(expected_diagnostic "negated x86 address displacement must fit in signed 32 bits")
    elseif(case_number EQUAL 16)
        set(expected_diagnostic "only one displacement may be added")
    elseif(case_number EQUAL 17 OR case_number EQUAL 18)
        set(expected_diagnostic "every referenced label must have exactly one definition")
    else()
        set(expected_diagnostic "constant expression")
    endif()
    if(NOT "${invalid_stdout}${invalid_stderr}" MATCHES "${expected_diagnostic}")
        message(FATAL_ERROR "Invalid operand case ${case_number} failed for an unexpected reason:\n${invalid_stdout}${invalid_stderr}")
    endif()
endforeach()

execute_process(
    COMMAND "${COMPILER}" -std=c++20 -O2 -fno-exceptions "-I${INCLUDE_DIR}"
        -DSTATIC_ASM_INVALID_CASE=7 -c "${SOURCE}"
        -o "${OUTPUT_DIR}/invalid_operand_no_exceptions.o"
    RESULT_VARIABLE no_exceptions_invalid_result
    OUTPUT_VARIABLE no_exceptions_invalid_stdout
    ERROR_VARIABLE no_exceptions_invalid_stderr
)
if(no_exceptions_invalid_result EQUAL 0 OR
    NOT "${no_exceptions_invalid_stdout}${no_exceptions_invalid_stderr}" MATCHES "constant expression")
    message(FATAL_ERROR "Invalid qword immediate was not rejected in a no-exceptions build:\n${no_exceptions_invalid_stdout}${no_exceptions_invalid_stderr}")
endif()
