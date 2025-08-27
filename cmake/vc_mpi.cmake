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
# MPI Configuration
#
#
include_guard()
vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")

vc_message_var(VOROCRUST_ENABLE_MPI)

set(VOROCRUST_HAVE_MPICXX_VAR "NO_MPI")

if(VOROCRUST_ENABLE_MPI)

    find_package(MPI REQUIRED)

    # Sets:
    #       (3.0 compatibility)
    vc_message_var(MPI_FOUND)
    vc_message_var(MPI_CXX_FOUND)
    vc_message_var(MPI_CXX_VERSION)
    vc_message_var(MPI_CXX_COMPILER)
    vc_message_var(MPI_CXX_COMPILER_FLAGS)
    vc_message_var(MPI_CXX_INCLUDE_PATH)
    vc_message_var(MPI_CXX_LIBRARIES)
    vc_message_var(MPI_CXX_LINK_FLAGS)
    vc_message_var(MPIEXEC_EXECUTABLE)
    vc_message_var(MPIEXEC_NUMPROC_FLAGS)
    vc_message_var(MPIEXEC_MAX_NUMPROCS)

    if(MPI_CXX_FOUND)
        set(VOROCRUST_HAVE_MPICXX_VAR "USE_MPI")
        include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})
    else()
        # MPI was requested, but we didn't find it.
        message(WARNING "${TagPREFIX} MPI was requested but was not found.")
    endif()

endif()

vc_message_var(VOROCRUST_HAVE_MPICXX_VAR)

# ------------------------------------------------
# Check for configuration failure conditions
# ------------------------------------------------
if(VOROCRUST_ENABLE_MPI)
    if(NOT MPI_CXX_FOUND)
        # User wants MPI enabled, but cmake can't find an MPI C++ compiler.
        message(FATAL_ERROR "Error: No MPI C++ compiler found, but configuration requested it.")
    endif()
endif()


# MPI settings and dependencies
macro(VOROCRUST_TPLADD_MPI TARGET)
    if(VOROCRUST_ENABLE_MPI)
        target_include_directories(${TARGET} PUBLIC ${MPI_CXX_INCLUDE_PATH})
        target_compile_options(${TARGET}     PUBLIC ${MPI_CXX_COMPILE_FLAGS})
        target_link_libraries(${TARGET}      PUBLIC ${MPI_CXX_LIBRARIES})
    endif()
endmacro()


vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")
