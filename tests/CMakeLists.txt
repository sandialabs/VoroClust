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
include(CTest)

vc_print_banner("BEGIN: ${CMAKE_CURRENT_LIST_FILE}")


#==============================================
# Testing
#==============================================
vc_message_var(PROJECT_BINARY_DIR)
vc_message_var(PROJECT_SOURCE_DIR)


#---------------------------
# BEGIN Tests for `vc_clustering`
#---------------------------
if(VOROCRUST_BUILD_CLUSTERING)

    get_property(VOROCRUST_APPNAME_VCCLUSTERING GLOBAL PROPERTY "VOROCRUST_APPNAME_vc_clustering")
    vc_message("--")
    vc_message("-- Adding tests for ${VOROCRUST_APPNAME_VCCLUSTERING}")
    vc_message("--")


    #-----------
    # Basic VoroClust Test
    #-----------
    set(VoroClustFiles config.in noisy_moons.csv run_clustering.sh verify_results.py)
    voroclust_add_basic_voroclust_test("BasicClusteringTest" "vc_clustering" "BasicClusteringTest" "config.in" "${VoroClustFiles}")
    set_tests_properties("BasicClusteringTest" PROPERTIES TIMEOUT 7200)

endif(VOROCRUST_BUILD_CLUSTERING)
#---------------------------
# END Tests for `vc_clustering`
#---------------------------

vc_print_banner("COMPLETE: ${CMAKE_CURRENT_LIST_FILE}")
