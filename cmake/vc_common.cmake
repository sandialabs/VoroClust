# BSD 2-Clause License
#
# Copyright (c) 2025, Sandia National Laboratories
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Common Configuration Options
#
#
include_guard(GLOBAL)
include(vc_functions)

vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")


# TODO: Verify version of CMake this is available for before enabling
#include(ProcessorCount)
#ProcessorCount(NUM_CORES)
#math(EXPR NUM_CORES "${NUM_CORES} - 2" )
#vc_message("Detected ${NUM_CORES} cores.")

# Configure destination paths in the Build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Set C++ Standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#========================================
# Global CMake Options
#========================================
set(BUILD_SHARED_LIBS    OFF CACHE BOOL "Build shared libraries?")

set(TPL_BUILD_ROOT_DIR   ${CMAKE_CURRENT_BINARY_DIR}/TPL_BUILD)
set(TPL_INSTALL_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/TPL)


# Macro: VOROCRUST_CHECK_COMPILER_VERSION
#
# Checks the compiler and version for known failure conditions.
# related to compiler type and version, etc. For example, Apple's
# built-in C++ compiler is Clang but Apple doesn't include
# OpenMP, and since VoroCrust requires OpenMP currently we
# want to error at configure time rather than halfway into a build.
#
macro( VOROCRUST_CHECK_COMPILER_VERSION )
    vc_message("BEGIN: Compiler compatibility check")
    vc_message("  Compiler type   : ${CMAKE_CXX_COMPILER_ID}")
    vc_message("  Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
            vc_message("GCC version too low, VoroCrust requires 5.x or higher.")
            message(FATAL_ERROR "${TagPREFIXERROR} COMPLETE: Compiler compatibility check FAILED")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        if(VOROCRUST_ENABLE_OPENMP)
            vc_message("Apple's Clang does not currently include OpenMP")
            vc_message("One solution might be to install 'gcc' via homebrew")
            vc_message("")
            vc_message("    $ brew install gcc")
            vc_message(" ")
            message(FATAL_ERROR "${TagPREFIXERROR} COMPLETE: Compiler compatibility check FAILED")
        endif()
    endif()
    vc_message("COMPLETE: Compiler compatibility check OK")
endmacro()



# Macro: PRINT_CMAKE_VARS
#
# Simple macro to just print out useful CMake Variables.
#
macro( PRINT_CMAKE_VARS )
    vc_message_var(PROJECT_NAME)
    vc_message_var(PROJECT_SOURCE_DIR)
    vc_message_var(PROJECT_BINARY_DIR)
    vc_message_var(CMAKE_BINARY_DIR)
    vc_message_var(CMAKE_CURRENT_SOURCE_DIR)
    vc_message_var(BUILD_SHARED_LIBS)
    vc_message_var(CMAKE_CXX_COMPILER_ID)
    vc_message_var(CMAKE_INSTALL_PREFIX)
    vc_message_var(CMAKE_MODULE_PATH)
    vc_message_var(CMAKE_PROJECT_NAME)
    vc_message_var(CMAKE_SYSTEM_NAME)
    vc_message_var(TPL_BUILD_ROOT_DIR)
    vc_message_var(TPL_INSTALL_ROOT_DIR)
    vc_message("---------------")
    vc_message_var(VOROCRUST_ENABLE_OPENMP)
    vc_message_var(VOROCRUST_ENABLE_WARNINGS)
    vc_message_var(VOROCRUST_VERBOSE_CMAKE)
    vc_message("---------------")
    vc_message("")
endmacro()



# Macro : ENABLE_CXXFLAG_IF_SUPPORTED
# Enable a given CXX flag if the compiler supports it.
# Usage:
#   ENABLE_CXXFLAG_IF_SUPPORTED( variable flag )
#
include(CheckCXXCompilerFlag)

macro(ENABLE_CXXFLAG_IF_SUPPORTED variable flag)
    string(REGEX REPLACE "^-" "" flagName "${flag}")
    check_cxx_compiler_flag("${flag}" HAVE_FLAG_${flagName})
    if(HAVE_FLAG_${flagName})
        set(${variable} "${${variable}} ${flag}" CACHE STRING "" FORCE)
    endif()
endmacro()



# Function that gets the lsb_release information if we're on linux
# to pull out distribution and version information.
function(get_linux_lsb_release_information)
    find_program(LSB_RELEASE_EXEC lsb_release)
    if(NOT LSB_RELEASE_EXEC)
        message(FATAL_ERROR "Could not detect lsb_release executable, can not gather required information")
    endif()

    execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --id OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --release OUTPUT_VARIABLE LSB_RELEASE_VERSION_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --codename OUTPUT_VARIABLE LSB_RELEASE_CODENAME_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(LSB_RELEASE_ID_SHORT "${LSB_RELEASE_ID_SHORT}" PARENT_SCOPE)
    set(LSB_RELEASE_VERSION_SHORT "${LSB_RELEASE_VERSION_SHORT}" PARENT_SCOPE)
    set(LSB_RELEASE_CODENAME_SHORT "${LSB_RELEASE_CODENAME_SHORT}" PARENT_SCOPE)
endfunction()

vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")


