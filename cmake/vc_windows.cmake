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
# CMake system file for Windows systems
#
#
include(vc_functions)

vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")

if(NOT WIN32)
    vc_message("Windows (WIN32) was NOT detected")
    vc_message("COMPLETE: ${CMAKE_CURRENT_SOURCE_DIR}/vc_windows.cmake")
    vc_message(" ")
    return()
endif()

# Windows Specific flags (MSVC)
vc_message("System Detected: WIN32")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -I/usr/local/include"
    CACHE STRING "omp include")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:experimental")

set(CMAKE_EXE_LINKER_FLAGS "-lomp -L/usr/local/lib/omp" CACHE STRING "omp lib")

# SEACAS + TPLs need -ldl (TODO: what is the windows equivalent?)
set(VC_LINKFLAG_LIBDL "")


vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")
