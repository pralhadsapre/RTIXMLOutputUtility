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
        routing_service
    )

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/")
link_directories("${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}/")

# Create the executable for RTI_XMLOutputUtility
add_executable(RTI_XMLOutputUtility
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RTI_XMLOutputUtility.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLineArgumentParser.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/XMLHelper.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Common.c")

target_link_libraries(RTI_XMLOutputUtility
    RTIConnextDDS::c_api
    RTIConnextDDS::routing_service_c
    rtixml2)

add_custom_command(
    TARGET RTI_XMLOutputUtility POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/helper_script/formatter.py
            ${CMAKE_CURRENT_BINARY_DIR}/formatter.py)