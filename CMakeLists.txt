cmake_minimum_required(VERSION 3.19)
project(static_asm
    VERSION 1.0.0
    DESCRIPTION "A C++20 compile-time x86 assembler library"
    LANGUAGES CXX
)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Determine if this is a top-level project or included via add_subdirectory/FetchContent
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(STATIC_ASM_MAIN_PROJECT ON)
else()
    set(STATIC_ASM_MAIN_PROJECT OFF)
endif()

# Options - tests/examples only build by default for the main project
option(STATIC_ASM_BUILD_TESTS "Build tests" ${STATIC_ASM_MAIN_PROJECT})
option(STATIC_ASM_BUILD_EXAMPLES "Build examples (requires Clang)" OFF)

# Create header-only library interface
add_library(static_asm INTERFACE)
add_library(static_asm::static_asm ALIAS static_asm)

target_include_directories(static_asm INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Require C++20
target_compile_features(static_asm INTERFACE cxx_std_20)

# Detect if using clang-cl (Clang with MSVC frontend)
set(USING_CLANG_CL FALSE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    set(USING_CLANG_CL TRUE)
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT USING_CLANG_CL)
    target_compile_options(static_asm INTERFACE
        -Wall -Wextra
    )
    # Suppress warnings for clang pragmas when using GCC
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(static_asm INTERFACE
            -Wno-unknown-pragmas
        )
    endif()
    # MinGW specific
    if(MINGW)
        target_compile_options(static_asm INTERFACE
            -Wa,-mbig-obj
        )
    endif()
elseif(MSVC OR USING_CLANG_CL)
    target_compile_options(static_asm INTERFACE
        /W4 /permissive- /Zc:__cplusplus /bigobj
        /Gy  # Enable function-level linking to handle duplicate COMDATs
        /wd4068  # Disable unknown pragma warning (we use #pragma clang diagnostic)
        /wd4244  # Disable conversion warning (intentional truncation of immediates)
    )
endif()

# Tests
if(STATIC_ASM_BUILD_TESTS)
    enable_testing()

    # Format check (optional - runs if clang-format is found)
    find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-19 clang-format-18)
    if(CLANG_FORMAT_EXE)
        file(GLOB_RECURSE ALL_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp
        )
        add_test(
            NAME format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCE_FILES}
        )
        set_tests_properties(format-check PROPERTIES LABELS "lint")
    endif()

    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Test sources
    set(TEST_SOURCES
        tests/test_main.cpp
        tests/test_alu.cpp
        tests/test_asm_block.cpp
        tests/test_bitscan.cpp
        tests/test_bt.cpp
        tests/test_call.cpp
        tests/test_cmov.cpp
        tests/test_jcc.cpp
        tests/test_jcc_near.cpp
        tests/test_jmp.cpp
        tests/test_lea.cpp
        tests/test_mov.cpp
        tests/test_muldiv.cpp
        tests/test_imul_multi.cpp
        tests/test_nop.cpp
        tests/test_pop.cpp
        tests/test_push.cpp
        tests/test_ret.cpp
        tests/test_shellcode.cpp
        tests/test_shift.cpp
        tests/test_sib.cpp
        tests/test_string.cpp
        tests/test_test.cpp
        tests/test_unary.cpp
        tests/test_xchg.cpp
        tests/test_generated.cpp
        tests/test_movzx_movsx.cpp
        tests/test_system.cpp
    )

    add_executable(static_asm_tests ${TEST_SOURCES})
    target_link_libraries(static_asm_tests PRIVATE
        static_asm
        GTest::gtest_main
    )

    # Compiler-specific test flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT USING_CLANG_CL)
        target_compile_options(static_asm_tests PRIVATE -O2 -Werror)
        if(MINGW)
            target_compile_options(static_asm_tests PRIVATE -Wa,-mbig-obj)
        endif()
    elseif(MSVC OR USING_CLANG_CL)
        # Only use /O2 in Release mode; Debug mode uses /RTC1 which conflicts with /O2
        target_compile_options(static_asm_tests PRIVATE /bigobj /WX)
        # Disable C4068 (unknown pragma) - we use #pragma clang diagnostic
        # Disable C4244 (conversion) - intentional truncation of immediates to register size
        target_compile_options(static_asm_tests PRIVATE /wd4068 /wd4244)
        target_compile_options(static_asm_tests PRIVATE $<$<CONFIG:Release>:/O2>)
        # Enable inline expansion in Debug mode to avoid duplicate COMDAT errors
        # from header-only template library (inline constexpr functions)
        target_compile_options(static_asm_tests PRIVATE $<$<CONFIG:Debug>:/Ob1>)
    endif()

    include(GoogleTest)
    gtest_discover_tests(static_asm_tests
        DISCOVERY_TIMEOUT 30  # MSVC Debug builds need more time for test discovery
    )
endif()

# Examples
if(STATIC_ASM_BUILD_EXAMPLES)
    add_executable(static_asm_example_basic examples/example_basic.cpp)
    target_link_libraries(static_asm_example_basic PRIVATE static_asm)

    add_executable(static_asm_example_basic_bis examples/example_basic_bis.cpp)
    target_link_libraries(static_asm_example_basic_bis PRIVATE static_asm)

    add_executable(static_asm_example_opaque_compute examples/example_opaque_compute.cpp)
    target_link_libraries(static_asm_example_opaque_compute PRIVATE static_asm)

    add_executable(static_asm_example_shellcode examples/example_shellcode.cpp)
    target_link_libraries(static_asm_example_shellcode PRIVATE static_asm)

    # Example-specific flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(static_asm_example_basic PRIVATE -O2)
        target_compile_options(static_asm_example_basic_bis PRIVATE -O2)
        target_compile_options(static_asm_example_opaque_compute PRIVATE -O2)
        target_compile_options(static_asm_example_shellcode PRIVATE -O2)
    elseif(MSVC)
        target_compile_options(static_asm_example_basic PRIVATE /O2)
        target_compile_options(static_asm_example_basic_bis PRIVATE /O2)
        target_compile_options(static_asm_example_opaque_compute PRIVATE /O2)
        target_compile_options(static_asm_example_shellcode PRIVATE /O2)
    endif()
endif()

# Installation rules (only for main project)
if(STATIC_ASM_MAIN_PROJECT)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Install target
    install(TARGETS static_asm
        EXPORT static_asm-targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install export targets
    install(EXPORT static_asm-targets
        FILE static_asm-targets.cmake
        NAMESPACE static_asm::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/static_asm
    )

    # Generate package config
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/static_asm-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/static_asm-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/static_asm
    )

    # Generate version file
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/static_asm-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    # Install config files
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/static_asm-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/static_asm-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/static_asm
    )
endif()
