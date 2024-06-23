# ------------------------------------------------------------------------------
# Project description
#
# Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)

project(
    presheaf
    VERSION 0.0.1
    LANGUAGES CXX C
)

# ------------------------------------------------------------------------------
# Compiler info
# ------------------------------------------------------------------------------

message(STATUS "C Compiler executable: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler executable: ${CMAKE_CXX_COMPILER}")
message(STATUS "Linker executable: ${CMAKE_LINKER}")

# ------------------------------------------------------------------------------
# Codebase compilation options
#
# You can assign to these options via `-D[OPTION NAME]=[On/Off]`.
# ------------------------------------------------------------------------------

# Code behaviour
option(PSH_NAMESPACED_TYPES     "All fundamental types (u32, i64, etc) should be namespaced"     ON)
option(PSH_DEBUG                "Compile in debug mode (enables all other flags)"               OFF)
option(PSH_ENABLE_LOGGING       "Compile with logging"                                          OFF)
option(PSH_ENABLE_ASSERTS       "Compile with assert checks"                                    OFF)
option(PSH_CHECK_BOUNDS         "Compile with bound check"                                      OFF)
option(PSH_CHECK_MEMCPY_OVERLAP "Checks if memcpy is called with overlapping regions of memory" OFF)
option(PSH_DISABLE_ANSI_COLORS  "Don't use ANSI colors for logging"                             OFF)

# Build options
option(PSH_BUILD_TEST "Build the entire suite of library tests" OFF)

# ------------------------------------------------------------------------------
# Tooling integration
# ------------------------------------------------------------------------------

# Enable Clangd LSP integration with the project
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Resulting compiler and linker flags for the build
# ------------------------------------------------------------------------------

set(PSH_CXX_DEFINES)       # Holds the resulting compiler #define flags
set(PSH_CXX_TEST_DEFINES)  # Holds the resulting compiler #define flags for the tests
set(PSH_CXX_FLAGS)         # Holds the resulting compiler flags for the build
set(PSH_CXX_SAN_FLAGS)     # Holds the resulting sanitizer flags for the build

# ------------------------------------------------------------------------------
# Compile-time "define" options
# ------------------------------------------------------------------------------

list(
    APPEND PSH_OPTIONS
        PSH_NAMESPACED_TYPES
        PSH_DEBUG
        PSH_ENABLE_LOGGING
        PSH_ENABLE_ASSERTS
        PSH_CHECK_BOUNDS
        PSH_CHECK_MEMCPY_OVERLAP
        PSH_DISABLE_ANSI_COLORS
)

if(UNIX)
    set(PSH_DEFINE_PREFIX "-D")
elseif(MSVC)
    set(PSH_DEFINE_PREFIX "/D")
endif()

# Register the enabled flags for the compilation.
foreach(opt IN LISTS PSH_OPTIONS)
    if(${opt})
        set(PSH_CXX_DEFINES ${PSH_CXX_DEFINES} "${PSH_DEFINE_PREFIX}${opt}=On")
    endif()
endforeach()

# MSVC says that fopen is unsafe... cringe
if(MSVC)
    set(PSH_CXX_DEFINES ${PSH_CXX_DEFINES} "${PSH_DEFINE_PREFIX}_CRT_SECURE_NO_WARNINGS")
endif()

# Enable all debug facilities for the tests.
set(PSH_CXX_TEST_DEFINES ${PSH_CXX_TEST_DEFINES} "${PSH_DEFINE_PREFIX}PSH_DEBUG=On")

# ------------------------------------------------------------------------------
# Compiler flags
#
# Here we'll have shared flags in the form `__PSH_COMMON_CXX_FLAGS_[COMPILERS]`
# and the variables with `PSH_[COMPILER]_CXX_FLAGS` will be the final result of
# the list of flags for the corresponding compiler.
# ------------------------------------------------------------------------------

# ---- Unix GCC and Clang ----

set(
    __PSH_COMMON_CXX_FLAGS_GCC_CLANG
    "-std=c++20"
    "-Wall"
    "-Wextra"
    "-pedantic"
    "-Wuninitialized"
    "-Wswitch"
    "-Wcovered-switch-default"
    "-Wshadow"
    "-Wcast-align"
    "-Wold-style-cast"
    "-Wpedantic"
    "-Wconversion"
    "-Wsign-conversion"
    "-Wnull-dereference"
    "-Wdouble-promotion"
    "-Wmisleading-indentation"
    "-Wformat=2"

    "-Wno-unused-variable"

    "-fno-rtti"
    "-fno-exceptions"
    "-fno-cxx-exceptions"
    "-fcolor-diagnostics"
    "-fno-force-emit-vtables"
)

# Final compiler flags
set(PSH_GCC_CXX_FLAGS   ${__PSH_COMMON_CXX_FLAGS_GCC_CLANG})
set(PSH_CLANG_CXX_FLAGS ${__PSH_COMMON_CXX_FLAGS_GCC_CLANG})

# ---- MSVC and Clang-cl ----

set(
    __PSH_COMMON_CXX_FLAGS_MSVC_CLANGCL
    "/std:c++20"
    "/fp:except-"   # Disable exceptions.
    "/GR-"          # Disable emission of RTTI data.
    "/GA"           # Optimize for Windows applications.
)
set(
    PSH_COMMON_CXX_DEBUG_FLAGS_MSVC_CLANGCL
    "/Zi"   # Debug info.
    "/Ob0"  # Disable inlining.
    "/Oy-"  # Disable frame pointer optimization.
)
set(
    PSH_COMMON_CXX_RELEASE_FLAGS_MSVC_CLANGCL
    "/Ob2"  # Inline functions as deemed beneficial by the compiler.
)

# Final compiler flags
set(
    PSH_MSVC_CXX_FLAGS ${__PSH_COMMON_CXX_FLAGS_MSVC_CLANGCL}
    "/EHc"   # Extern "C" defaults to nothrow.
)
set(
    PSH_CLANGCL_CXX_FLAGS ${__PSH_COMMON_CXX_FLAGS_MSVC_CLANGCL}
    "-fcolor-diagnostics"
    "-nobuiltininc"
    "/W3"
    "-Wuninitialized"
    "-Wswitch"
    "-Wcovered-switch-default"
    "-Wshadow"
    "-Wold-style-cast"
    "-Wpedantic"
    "-Wconversion"
    "-Wsign-conversion"
    "-Wnull-dereference"
    "-Wdouble-promotion"
    "-Wmisleading-indentation"
    "-Wcast-align"
    "-Wformat=2"
    "-Wno-unused-variable"
)

# ------------------------------------------------------------------------------
# Sanitizer flags
#
# Here we'll have shared flags in the form `__PSH_COMMON_SAN_CXX_FLAGS_[COMPILERS]`
# and the variables with `PSH_[COMPILER]_CXX_SAN_FLAGS` will be the final result
# of the variables.
# ------------------------------------------------------------------------------

# ---- GCC and Clang ----

set(
    __PSH_COMMON_CXX_SAN_FLAGS_GCC_CLANG
    "-fsanitize=address"
    "-fsanitize=pointer-compare"
    "-fsanitize=pointer-subtract"
    "-fsanitize=undefined"
    "-fstack-protector-strong"
    "-fsanitize=leak"
)
set(PSH_GCC_CXX_SAN_FLAGS   ${__PSH_COMMON_CXX_SAN_FLAGS_GCC_CLANG})
set(PSH_CLANG_CXX_SAN_FLAGS ${__PSH_COMMON_CXX_SAN_FLAGS_GCC_CLANG})

# ---- MSVC and Clang-cl ----

set(
    __PSH_COMMON_CXX_SAN_FLAGS_MSVC_CLANGCL
    # TODO: add "/fsanitize=address" flag
)
set(PSH_MSVC_CXX_SAN_FLAGS    ${__PSH_COMMON_CXX_SAN_FLAGS_MSVC_CLANGCL})
set(PSH_CLANGCL_CXX_SAN_FLAGS ${__PSH_COMMON_CXX_SAN_FLAGS_MSVC_CLANGCL})

# ------------------------------------------------------------------------------
# Compiler flags setup per compiler
# ------------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "GCC")
    set(PSH_CXX_FLAGS ${PSH_GCC_CXX_FLAGS})
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang
    if(UNIX)
        set(PSH_CXX_FLAGS ${PSH_CXX_FLAGS} ${PSH_CLANG_CXX_FLAGS})
    endif(UNIX)

    # Clang-cl
    if(MSVC)
        set(PSH_CXX_FLAGS ${PSH_CXX_FLAGS} ${PSH_CLANGCL_CXX_FLAGS})
    endif(MSVC)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(PSH_CXX_FLAGS ${PSH_MSVC_CXX_FLAGS})
endif()

# Set debug/release flags for both MSVC and Clang-cl
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(PSH_CXX_FLAGS     ${PSH_CXX_FLAGS}     ${PSH_COMMON_CXX_DEBUG_FLAGS_MSVC_CLANGCL})
        set(PSH_CXX_SAN_FLAGS ${PSH_CXX_SAN_FLAGS} ${PSH_CLANGCL_CXX_SAN_FLAGS})
    else()
        set(PSH_CXX_FLAGS ${PSH_CXX_FLAGS} ${PSH_COMMON_CXX_RELEASE_FLAGS_MSVC_CLANG_CL})
    endif()
endif()

# ------------------------------------------------------------------------------
# Inform the resulting compiler and sanitizer flags
# ------------------------------------------------------------------------------

message(STATUS "Presheaf C++ compiler flags: ${PSH_CXX_FLAGS}")
message(STATUS "Presheaf C++ sanitizer flags: ${PSH_CXX_SAN_FLAGS}")

# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------

set(
    PSH_SOURCES
    "${PROJECT_SOURCE_DIR}/src/arena.cc"
    "${PROJECT_SOURCE_DIR}/src/log.cc"
    "${PROJECT_SOURCE_DIR}/src/mem_utils.cc"
    "${PROJECT_SOURCE_DIR}/src/memory_manager.cc"
    "${PROJECT_SOURCE_DIR}/src/stack.cc"
    "${PROJECT_SOURCE_DIR}/src/streams.cc"
    "${PROJECT_SOURCE_DIR}/src/string.cc"
    "${PROJECT_SOURCE_DIR}/src/vec.cc"
)

# ------------------------------------------------------------------------------
# Where the products are going
# ------------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# ------------------------------------------------------------------------------
# Library.
# ------------------------------------------------------------------------------

add_library(presheaf ${PSH_SOURCES})
target_compile_options(presheaf PRIVATE ${PSH_CXX_DEFINES} ${PSH_CXX_FLAGS} ${PSH_CXX_SAN_FLAGS})
target_link_libraries(presheaf PUBLIC ${PSH_CXX_SAN_FLAGS})
target_include_directories(presheaf PUBLIC "${PROJECT_SOURCE_DIR}/include")

# ------------------------------------------------------------------------------
# Library tests
# ------------------------------------------------------------------------------


if(PSH_BUILD_TESTS)
    list(
        APPEND TESTS
            "test_types"
            "test_allocators"
            "test_memory_manager"
            "test_dynarray"
            "test_string"
            "test_bit"
            "test_vec"
    )

    foreach(t IN LISTS TESTS)
        add_executable(${t} "${PROJECT_SOURCE_DIR}/tests/${t}.cc")
        target_compile_options(${t} PRIVATE ${PSH_CXX_TEST_DEFINES} ${PSH_CXX_FLAGS} ${PSH_SAN_FLAGS})
        target_link_libraries(${t} PUBLIC ${PSH_SAN_FLAGS} presheaf)
        add_test(NAME ${t} COMMAND ${t})
    endforeach()
endif()
