#
# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
#
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form
#  only for use with RTI products.  The Software is provided "as is", with no
#  warranty of any type, including any warranty for fitness for any purpose.
#  RTI is under no obligation to maintain or support the Software.  RTI shall
#  not be liable for any incidental or consequential damages arising out of the
#  use or inability to use the software.


cmake_minimum_required(VERSION 3.5)
project ("RTI XML Value Output Utility")

if(NOT CONNEXTDDS_DIR)
    set(msg "Please provide the top level folder location for your Connext DDS installation. "
            "You need v6.0.0 and above for using this utility. Use the following option "
            "-DCONNEXTDDS_DIR=<your installation path>.")
    message(FATAL_ERROR ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
)

message("Compiler Version - ${CMAKE_C_COMPILER_VERSION}")
message("Operating System - ${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND
    CMAKE_C_COMPILER_VERSION VERSION_GREATER "4.6.0")
    set(CONNEXTDDS_EXTERNAL_LIBS
        -Wl,--no-as-needed  ${CONNEXTDDS_EXTERNAL_LIBS})
    message("CONNEXTDDS_EXTERNAL_LIBS Updated")
endif()

set(RTI_XMLOUTPUTUTILITY_C_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RTI_XMLOutputUtility.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLineArgumentParser.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/XMLHelper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Common.c")

find_library(rtixml2_library
    NAMES rtixml2
    PATHS "${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}/"
)

# Create the executable for RTI_XMLOutputUtility
add_executable(RTI_XMLOutputUtility
    ${RTI_XMLOUTPUTUTILITY_C_SOURCES}
)

target_link_libraries(RTI_XMLOutputUtility
    RTIConnextDDS::c_api
    ${rtixml2_library}
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(RTI_XMLOutputUtility
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src")