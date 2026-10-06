if(NOT DEFINED COMPILER OR NOT DEFINED SOURCE OR NOT DEFINED INCLUDE_DIR OR NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "Missing emit compile-check arguments")
endif()

execute_process(
    COMMAND "${COMPILER}" -std=c++20 -O2 "-I${INCLUDE_DIR}" -DSTATIC_ASM_TEST_NO_EMIT
        -c "${SOURCE}" -o "${OUTPUT_DIR}/emit_control.o"
    RESULT_VARIABLE control_result
    OUTPUT_VARIABLE control_stdout
    ERROR_VARIABLE control_stderr
)
if(NOT control_result EQUAL 0)
    message(FATAL_ERROR "Emit control source failed to compile:\n${control_stdout}${control_stderr}")
endif()

execute_process(
    COMMAND "${COMPILER}" -std=c++20 -O2 "-I${INCLUDE_DIR}"
        -c "${SOURCE}" -o "${OUTPUT_DIR}/emit_unsupported.o"
    RESULT_VARIABLE emit_result
    OUTPUT_VARIABLE emit_stdout
    ERROR_VARIABLE emit_stderr
)
if(emit_result EQUAL 0)
    message(FATAL_ERROR "core::emit compiled for an unsupported target")
endif()
if(NOT "${emit_stdout}${emit_stderr}" MATCHES "core::emit.*x86")
    message(FATAL_ERROR "core::emit failed for an unexpected reason:\n${emit_stdout}${emit_stderr}")
endif()
