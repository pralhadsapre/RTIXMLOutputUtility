/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef commandlineargumentparser_h
#define commandlineargumentparser_h

#include "Common.h"

#define SPLIT_STRING_QOS_TAG "/"
#define SPLIT_STRING_PROFILE_PATH "::"

struct CommandLineArguments {
    char *user_file;
    char *output_file;
    char *qos_library;
    char *qos_profile;
    char *qos_type;
    char *topic_name;
    char *query;
};

void CommandLineArguments_initialize(struct CommandLineArguments *cmd_args);

void CommandLineArguments_finalize(struct CommandLineArguments *cmd_args);

RTI_Retval parse_arguments(
        int argc, 
        char *argv[], 
        struct CommandLineArguments *output_values);

void print_help();

void print_arguments(struct CommandLineArguments *values);

#endif /* commandlineargumentparser_h */