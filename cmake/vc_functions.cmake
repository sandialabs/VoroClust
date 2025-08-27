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
include_guard(GLOBAL)
#
# Functions and Macros
#

#
# Set up some color codes for CMake Output
#
if((NOT WIN32) AND CMAKE_COLOR_CMAKE)
  string(ASCII 27 Esc)
  set(ColorReset  "${Esc}[m")
  set(ColorBold   "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()


# Set some message prefix tags
set(TagPREFIX      "${Green}[VC]${ColorReset}")
set(TagVERBOSE     "${TagPREFIX} ${Yellow}[VERBOSE]${ColorReset}")
set(TagPREFIXERROR "${Red}[VC]${ColorReset}")


# Helper Macro: Print a simple "VoroCrust" information message
#
# @param TEXT - The text to display
macro(vc_message TEXT)
    message(STATUS "${TagPREFIX} ${TEXT}")
endmacro()


# Helper Macro: Format and print a variable to the log
#
# @param TEXT - The text to display
macro(vc_message_var VAR)
    vc_message("-- ${Cyan}${VAR}${ColorReset} = ${Magenta}${${VAR}}${ColorReset}")
endmacro()


# Helper Macro: Print a simple "VoroCrust" message guarded by
#               VOROCRUST_VERBOSE_CMAKE.
#
# @param TEXT - The text to display
macro(vc_message_verbose TEXT)
    if(VOROCRUST_VERBOSE_CMAKE)
        message(STATUS "${TagVERBOSE} ${TEXT}")
    endif()
endmacro()


# Helper Macro: Print a banner to the console log
#
# @param TEXT - the text to display
macro(vc_print_banner TEXT)
    vc_message("${Green}--------------------------------------------------------------------------------${ColorReset}")
    vc_message("${Green}${TEXT}${ColorReset}")
    vc_message("${Green}--------------------------------------------------------------------------------${ColorReset}")
endmacro()


# Helper Function: Appends a value to a global property (list).
#                  This is useful when setting certain properties down in
#                  one subdirectory of the overall project so that we can
#                  we can see it in an adjacent subdirectory.
#
#  Usage:
#     (setting): append_to_property("FOO" "FOOVALUE")
#     (reading): get_property(<VARNAME> GLOBAL PROPERTY "FOO")
#     In this example reading the property will save the value of the global
#     property "FOO" into <VARNAME>.
#
# @param PROPERTY_NAME - The name of the global property we wish to save or append to.
# @param VALUE         - The value to save or append to the property.
#
function(append_to_property PROPERTY_NAME VALUE)
    get_property(tmp GLOBAL PROPERTY ${PROPERTY_NAME})
    set_property(GLOBAL APPEND PROPERTY ${PROPERTY_NAME} ${VALUE})
endfunction()


# Helper Function: Enables a flag if it's supported with
#                  an extra check to only add the flag if we don't
#                  already have it in the flags.
#                  If the flag is CMAKE_CXX_FLAGS then we add a check to
#                  see that it's supported.
#
# @param ARGFLAGS  - The FLAG list to append to (i.e., "CMAKE_CXX_FLAGS")
# @param ARGOPTION - The FLAG to set. (i.e, "-fPIC")
#
# Example Usage:
#    enable_flag_if_supported_unique(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS}")
macro(ENABLE_FLAG_IF_SUPPORTED_UNIQUE ARGFLAGS ARGOPTION)
    set(_TMP "${${ARGFLAGS}}")
    string(REPLACE " " ";" _TMP "${_TMP}" )
    if(NOT "${ARGOPTION}" IN_LIST _TMP)
        if( "${ARGFLAGS}" STREQUAL "CMAKE_CXX_FLAGS" )
            enable_cxxflag_if_supported( ${ARGFLAGS} "${ARGOPTION}" )
        else()
            set(${ARGFLAGS} "${ARGFLAGS} ${ARGOPTION}")
        endif()
    endif()
    unset(_TMP)
endmacro()


# Helper Function: Copies data from the tests/datasets dir in the
#                  source repository into the tests/ directory under
#                  the build directory.
#
# @param TEST_NAME    - The TEST name. This will create a directory in
#                       tests/${TEST_NAME}/ within the build directory
#                       as the destination of the file copy.
# @param DATASET_NAME - The DATASET name. This is the source location of
#                       the datasets.
#                       This is in <souce root>/tests/datasets/${DATASET_NAME}/
# @param FILE_NAME    - The file that we're copying.
#
macro(vorocrust_copy_dataset TEST_NAME DATASET_NAME FILE_NAME)
    vc_message_verbose("vorocrust_copy_dataset()")
    vc_message_verbose("- TEST_NAME   : ${TEST_NAME}")
    vc_message_verbose("- DATASET_NAME: ${DATASET_NAME}")
    vc_message_verbose("- FILE_NAME   : ${FILE_NAME}")

    configure_file("${PROJECT_SOURCE_DIR}/tests/datasets/${DATASET_NAME}/${FILE_NAME}"
                   "${PROJECT_BINARY_DIR}/tests/${TEST_NAME}/${FILE_NAME}"
                   COPYONLY)
endmacro()


macro(voroclust_add_basic_voroclust_test TEST_NAME APPNAME DATASET_NAME CONFIG_IN FILE_LIST)
    vc_message("Add test ${TEST_NAME}")

    get_property(TEST_APPNAME GLOBAL PROPERTY "VOROCRUST_APPNAME_${APPNAME}")

    vc_message_verbose("vorocrust_add_monitoring_points_test()")
    vc_message_verbose("- TEST_NAME   : ${TEST_NAME}")
    vc_message_verbose("- DATASET_NAME: ${DATASET_NAME}")
    vc_message_verbose("- CONFIG_IN   : ${CONFIG_IN}")
    vc_message_verbose("- FILE_LIST   : ${FILE_LIST}")
    vc_message_verbose("- TEST_APPNAME: ${TEST_APPNAME}")

    vorocrust_copy_dataset(${TEST_NAME} ${DATASET_NAME} ${CONFIG_IN})
    foreach(FILE_I ${FILE_LIST})
        vorocrust_copy_dataset(${TEST_NAME} ${DATASET_NAME} ${FILE_I})
    endforeach()
    add_test(NAME ${TEST_NAME}
             WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/tests/${TEST_NAME}"
             COMMAND           ./run_clustering.sh
            )

endmacro()


# Helper Function: Add an optional subdirectory if it exists
#
# @param DIRNAME - The name of the subdirectory to add.
#
macro(add_optional_subdirectory_if_exists DIRNAME)
    if(EXISTS "${DIRNAME}")
        vc_message("${TagPREFIX} Add `${DIRNAME}`")
        add_subdirectory(${DIRNAME})
    else()
        vc_message("Add `${DIRNAME}` ${Red}SKIPPED (NOT FOUND)${ColorReset}")
    endif()
endmacro()


# Helper: Add a VoroCrust application
#
# @param EXENAME     - The name of the executable to generate.
#                      This is also the CMake TARGET for this app.
# @param SOURCES     - The list of sources for the application.
# @param INCLUDE_DIR - Any extra include directories to add.
# @param LIBRARIES   - Any extra libraries to add.
macro(VOROCLUST_ADD_APP EXENAME SOURCES INCLUDE_DIR LIBRARIES)
    vc_message("---------------------")
    vc_message("-- ADD APPLICATION --")
    vc_message("---------------------")
    vc_message("-- EXENAME     : ${EXENAME}")
    vc_message("-- INCLUDE_DIR : ${${INCLUDE_DIR}}")
    vc_message("-- LIBRARIES   : ${${LIBRARIES}}")

    append_to_property(VOROCRUST_BUILD_TARGETS "${EXENAME}")
    append_to_property("VOROCRUST_APPNAME_${EXENAME}" "${EXENAME}")

    # Set up main test executable
    add_executable(${EXENAME} ${${SOURCES}})
    target_include_directories(${EXENAME} PUBLIC ${${INCLUDE_DIR}})
    target_link_libraries(${EXENAME} PUBLIC ${${LIBRARIES}})

    # MPI settings and dependencies
    vorocrust_tpladd_mpi(${EXENAME})

    # Add dependencies (affects build order)
    if(VOROCRUST_VERSION_USES_GIT_SHA1)
        add_dependencies(${EXENAME} update_git_sha1)
    else()
        vc_message("-- Skip setting version string via git sha1 lookup")
    endif()
    add_dependencies(${EXENAME} libVoroClust)
    vc_message("--")
endmacro()


# Helper: Add a dependency to a target.
#
# @param TARGET     - The target name.
# @param DEPENDENCY - The dependency to add
macro(VOROCRUST_ADD_DEPENDENCY TARGET DEPENDENCY)
    vc_message("vc_add_dependencies(${TARGET} ${DEPENDENCY})")
    add_dependencies(${TARGET} ${DEPENDENCY})
    target_link_libraries(${TARGET} PUBLIC ${DEPENDENCY})
endmacro()

