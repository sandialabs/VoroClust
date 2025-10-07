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
cmake_minimum_required(VERSION 3.12.0)

#This path used when the python build is a part of the full vorocrust repository (in src/Clustering/python)
#set(SRC_PATH "..")

#This path used in standalone voroclust repository, when the python folder is moved from src to the toplevel
set(SRC_PATH "../src/Clustering")

set(target voroclust)
project(${target} LANGUAGES CXX)

file(GLOB CLUSTERING_SRCS 
    ${SRC_PATH}/*.cpp) 

find_package(OpenMP REQUIRED)

if(WIN32)
    #Almost certainly not the correct way to do this. If you know better, please change
    string(REPLACE "openmp" "openmp:experimental" OpenMP_CXX_FLAGS ${OpenMP_CXX_FLAGS})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

find_package(pybind11 REQUIRED)
pybind11_add_module(${target} voroclust/voroclust.cpp ${CLUSTERING_SRCS})
target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)



target_include_directories(${target}
    PUBLIC
    ${SRC_PATH}
    ./GeneratedHeaders/
)


## Assuming target is named yourpkg_core and you want it to import as yourpkg._core
#set_target_properties(${target} PROPERTIES
#  OUTPUT_NAME "voroclust"
#  PREFIX ""                               # no "lib" on Unix (pybind11_add_module does this for you)
#  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/voroclust"
#  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/voroclust"  # Windows
#  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/voroclust"  # static import libs on Windows
#)
## If you use a multi-config generator (VS/Xcode), set per-config outputs:
#foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
#  set_target_properties(${target} PROPERTIES
#    LIBRARY_OUTPUT_DIRECTORY_${cfg} "${CMAKE_CURRENT_SOURCE_DIR}/voroclust"
#    RUNTIME_OUTPUT_DIRECTORY_${cfg} "${CMAKE_CURRENT_SOURCE_DIR}/voroclust"
#    ARCHIVE_OUTPUT_DIRECTORY_${cfg} "${CMAKE_CURRENT_SOURCE_DIR}/voroclust"
#  )
#endforeach()
#

install(TARGETS voroclust
  LIBRARY DESTINATION voroclust
  RUNTIME DESTINATION voroclust
  ARCHIVE DESTINATION voroclust)
