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
# VoroCrust Clustering CMake Script
#
vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")

vc_message_var(VOROCRUST_BUILD_CLUSTERING)


if(VOROCRUST_BUILD_CLUSTERING)
    set(target libVoroClust)

    # Sources
    file(GLOB VOROCLUST_SRCS *.cpp)
    file(GLOB VOROCLUST_HDRS *.h)
    set(VOROCLUST_EXTRA_HEADERS "")

    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    # Generate Version.h from Version.h.in
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.h" @ONLY)
    list(APPEND VOROCLUST_EXTRA_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/Version.h")

    # Generate Config.h from Config.h.in
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Config.h" @ONLY)
    list(APPEND VOROCLUST_EXTRA_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/Config.h")


    vc_message_var(VOROCRUST_CLUSTERING_EXTRA_HEADERS)
    vc_message_var(CMAKE_CURRENT_BINARY_DIR)
    vc_message_var(CMAKE_BINARY_DIR)

    # Include the binary dir to get the generated file(s).
    include_directories("${CMAKE_CURRENT_BINARY_DIR}")

    add_library(${target}
        ${VOROCLUST_HDRS}
        ${VOROCLUST_SRCS}
        ${VOROCLUST_EXTRA_HEADERS}
    )
    target_include_directories(${target}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
    
    # MPI settings and dependencies
    vorocrust_tpladd_mpi(${target})

    # vc_clustering APP
    set(VOROCLUST_APP_SOURCES "voroclust.cxx")
    set(VOROCLUST_APP_LIBS libVoroClust)
    voroclust_add_app("voroclust"
            VOROCLUST_APP_SOURCES
            VOROCLUST_APP_INCLUDES
            VOROCLUST_APP_LIBS
            )

    add_optional_subdirectory_if_exists("${CMAKE_CURRENT_SOURCE_DIR}/tests")

endif(VOROCRUST_BUILD_CLUSTERING)

vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")
