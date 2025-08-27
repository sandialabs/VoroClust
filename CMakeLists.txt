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

cmake_minimum_required(VERSION 3.14.0)
# 3.12 is required due to Kokkos dependency.
# We may need higher though because 3.12 has issues with the installation targets
# but we have a guard around that currently.
project(VoroClust)


# Set C++11 Standard as the minimum required.
set(CMAKE_CXX_STANDARD "14" CACHE STRING "Set by VoroClust.")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Set by VoroClust")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Set by VoroClust")


#========================================
# Import cmake common helper file(s)
#========================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(vc_common)
include(vc_functions)



# Set Build Type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE
        STRING "Choose build type, options are: None Debug Release RelWithDebInfo MinSizeRel"
        FORCE)
endif()

#========================================
# VoroCrust Project Options
#========================================
option(VOROCRUST_VERBOSE_CMAKE "Enable verbosity in CMake output" OFF)

option(VOROCRUST_BUILD_CLUSTERING      "Build the VC_Clustering app"       ON)

option(VOROCRUST_ENABLE_OPENMP "Enable OpenMP for VoroCrust"                 ON)

option(VOROCRUST_ENABLE_LONG_TESTS "Enable long duration tests" OFF)

option(VOROCRUST_ENABLE_TPL_LOG_DOWNLOAD "Make ExternalPackage TPLs send download output to logs"  ON)
option(VOROCRUST_ENABLE_TPL_LOG_CONF     "Make ExternalPackage TPLs send configure output to logs" OFF)
option(VOROCRUST_ENABLE_TPL_LOG_BUILD    "Make ExternalPackage TPLs send build output to logs"     OFF)
option(VOROCRUST_ENABLE_TPL_LOG_TEST     "Make ExternalPackage TPLs send test output to logs"      OFF)
option(VOROCRUST_ENABLE_TPL_LOG_INST     "Make ExternalPackage TPLs send install output to logs"   ON)
mark_as_advanced(VOROCRUST_ENABLE_TPL_LOG_DOWNLOAD)
mark_as_advanced(VOROCRUST_ENABLE_TPL_LOG_CONF)
mark_as_advanced(VOROCRUST_ENABLE_TPL_LOG_BUILD)
mark_as_advanced(VOROCRUST_ENABLE_TPL_LOG_TEST)
mark_as_advanced(VOROCRUST_ENABLE_TPL_LOG_INST)

option(VOROCRUST_VERSION_USES_GIT_SHA1 "Set VERSION values by checking the Git SHA1" OFF)
mark_as_advanced(VOROCRUST_VERSION_USES_GIT_SHA1)

option(VOROCRUST_ENABLE_WARNINGS   "Enable a standard set of warnings flags" OFF)
mark_as_advanced(VOROCRUST_ENABLE_WARNINGS)


#========================================
# Initialize GLOBAL scope vars
#========================================
set(CMAKE_COLOR_CMAKE ON CACHE BOOL "Enable colorized CMake on non-windows platforms")
mark_as_advanced(CMAKE_COLOR_CMAKE)

#========================================
# Enable Testing Globally
#========================================
enable_testing()


#========================================
# Import configuration files
#========================================

include(vc_windows)
include(vc_unix)
include(vc_mpi)
include(vc_openmp)

if(VOROCRUST_VERSION_USES_GIT_SHA1)
    include(vc_git_tools)
else()
    vc_message("Skip setting version string via git sha1 lookup")
    # Set Default values for GIT_COMMIT_* vars
    set(GIT_COMMIT_SHA1 "UNKNOWN")
    set(GIT_COMMIT_DATE "UNKNOWN")
    vc_message_var(GIT_COMMIT_SHA1)
    vc_message_var(GIT_COMMIT_DATE)
endif()


# Include the src/ directory
add_subdirectory(src)

# Add src/ directory to the -I<INCLUDE> paths for compilation
include_directories(src)

# Optionally prints out some useful CMake vars
# if VOROCRUST_VERBOSE_CMAKE is enabled.
print_cmake_vars()

# Verify that the compiler is compatible with VoroCrust.
vorocrust_check_compiler_version()

# Include the Global tests subdirectory
add_subdirectory(tests)


#========================================
# Installation
#========================================
vc_print_banner("BEGIN: Configure Installation & Packaging Properties")
if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0" )

    get_property(VOROCRUST_BUILD_TARGETS GLOBAL PROPERTY "VOROCRUST_BUILD_TARGETS")

    set(VOROCRUST_COPYRIGHT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT.md")
    set(VOROCRUST_LICENSE_FILE   "")
    set(VOROCRUST_README_FILE    "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    install(TARGETS ${VOROCRUST_BUILD_TARGETS}
            RUNTIME DESTINATION "bin"
            LIBRARY DESTINATION "lib"
            ARCHIVE DESTINATION "lib"
            COMPONENT applications
            )

    # Add examples to the distribution package
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/datasets/README.md
            DESTINATION share/${PROJECT_NAME}/examples
            )
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/datasets/Clustering
            DESTINATION share/${PROJECT_NAME}/examples
            )
    install(FILES ${VOROCRUST_COPYRIGHT_FILE}
                  ${VOROCRUST_README_FILE}
            DESTINATION share/${PROJECT_NAME}
            )

    # Add project files
    install(FILES ${VOROCRUST_COPYRIGHT_FILE}
                  ${VOROCRUST_README_FILE}
            DESTINATION share/${PROJECT_NAME}
            )

    #========================================
    # BEGIN Packaging
    #========================================
    # See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Packaging-With-CPack#using-cmake-variables-to-configure-cpack
    #      for information useful for packaging on windows.
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VoroClust")
    set(CPACK_PACKAGE_VENDOR "Sandia National Laboratories")
    #set(CPACK_RESOURCE_FILE_LICENSE ${VOROCRUST_LICENSE_FILE})
    set(CPACK_RESOURCE_FILE_README  ${VOROCRUST_README_FILE})
    set(CPACK_VERBATIM_VARIABLES true)

    # Default generator is a ZIP file.
    # This can be changed via -G <GENERATOR> or -DCPACK_GENERATOR="ZIP;TGZ"
    set(CPACK_GENERATOR "ZIP")

    # Control filename for output package
    # See: https://cmake.org/cmake/help/v3.8/module/CPack.html#variable:CPACK_PACKAGE_FILE_NAME
    # Default configuration is "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}"
    string(TIMESTAMP TODAY "%Y%m%d")

    set(CPACK_PACKAGE_VERSION ${TODAY})

    set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

    if(APPLE)
        cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
        #vc_message("OS_RELEASE = ${OS_RELEASE}")
        set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${OS_RELEASE}-${CMAKE_SYSTEM_PROCESSOR}")
    endif(APPLE)

    vc_message_var(CMAKE_SYSTEM_NAME)

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        vc_message_var(CMAKE_SYSTEM_NAME)
        get_linux_lsb_release_information()

        vc_message_var(LSB_RELEASE_ID_SHORT)
        vc_message_var(LSB_RELEASE_VERSION_SHORT)
        set(CPACK_SYSTEM_NAME "${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_VERSION_SHORT}-${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    vc_message_var(CMAKE_SYSTEM_NAME)
    vc_message_var(CMAKE_SYSTEM_PROCESSOR)
    vc_message_var(CPACK_SYSTEM_NAME)

    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/VoroCrust.png")

    if (UNIX)
        set(CPACK_STRIP_FILES ${PROJECT_NAME})
        set(CPACK_SOURCE_STRIP_FILES "")
    endif()

    set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} "VoroClust Executable")
    include(CPack)
    #========================================
    # END Packaging
    #========================================

else()

    vc_message("Installation and Packaging disabled. To enable please upgrade CMake to 3.16 or greater.")

endif()

vc_print_banner("COMPLETE: Installation & Packaging Properties")


