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
# VoroClust src CMake
#
vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")

# Force CMake to re-run the vc_git_tools.cmake script every
# time we build VoroCrust to ensure the SHA1 being set is accurate.
# Note: This won't work if this is not a git clone -- this will be replaced
#       when VoroCrust starts having numbered releases.
if(VOROCRUST_VERSION_USES_GIT_SHA1)
    add_custom_target(update_git_sha1 COMMAND cmake -P "${CMAKE_SOURCE_DIR}/cmake/vc_git_tools.cmake")
else()
    vc_message("Skip version configuration via Git SHA1")
endif()

add_subdirectory(Clustering)

vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")

