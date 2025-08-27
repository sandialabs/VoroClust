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
# OpenMP Configuration
#
#
include(vc_functions)

vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")

include(vc_functions)

vc_message_var(VOROCRUST_ENABLE_OPENMP)

if(VOROCRUST_ENABLE_OPENMP)

    find_package(OpenMP)
    # Sets: OpenMP_C_FLAGS, OpenMP_CXX_FLAGS, OpenMP_Fortran_FLAGS, OPENMP_FOUND
    #       (3.0 compatibility)
    # Note: Not sure where the docs are for CMake 2.8.x versions
    # Note: OPENMP_FOUND is kept for backwards compatibility,
    # but cmake > 3.9.x defines OpenMP_<lang>_FOUND where <lang> is C, CXX, or FORTRAN
    if(OPENMP_FOUND)

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" CACHE STRING "" FORCE)

        if(WIN32)
            string(REPLACE "openmp" "openmp:experimental" OpenMP_CXX_FLAGS ${OpenMP_CXX_FLAGS})
        endif()

        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" CACHE STRING "" FORCE)
        enable_flag_if_supported_unique(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS}")

        set(VOROCRUST_HAVE_OPENMP_VAR "USE_OPEN_MP")

        vc_message_var(OPENMP_FOUND)

    else()
        # OpenMP was not found but we requested it.
        vc_message("${Red}WARNING: OpenMP was requested but not found${ColorReset}")
        #if(VOROCRUST_VERBOSE_CMAKE)
        #    message(WARNING "${TagVERBOSE} OpenMP was requested but not found.")
        #endif()
        set(VOROCRUST_HAVE_OPENMP_VAR "NO_OPEN_MP")
    endif()

else()

    # OpenMP is not requested.
    set(VOROCRUST_HAVE_OPENMP_VAR "NO_OPEN_MP")

endif()

vc_message_var(VOROCRUST_HAVE_OPENMP_VAR)

# Check for conditions that warrant failing CMake
if(VOROCRUST_ENABLE_OPENMP)
    if(NOT OPENMP_FOUND)
        # User wants OpenMP enabled, but cmake didn't find it.
        message(FATAL_ERROR "Error: OpenMP not found, it is currently a requirement for VoroCrust")
    endif()
endif()


vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")
