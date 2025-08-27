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

#This path used in standalone VoroClust repository, when the python folder is moved from src to the toplevel
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
pybind11_add_module(${target} voroclust.cpp ${CLUSTERING_SRCS})
target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)

target_include_directories(${target}
    PUBLIC
    ${SRC_PATH}
    ./GeneratedHeaders/
)
