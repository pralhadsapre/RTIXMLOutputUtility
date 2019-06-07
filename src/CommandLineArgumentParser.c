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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CommandLineArgumentParser.h"

const char *ARG_HELP[3] = {
        "-help", 
        "\t Displays all the options of the RTI_XMLOutputUtility", 
        ""};
const char *ARG_USER_FILE[3] = {
        "-qosFile", 
        "Absolute path of the QoS XML configuration file you want to analyse", 
        "OPTIONAL: The standard QoS XML files as defined in the User's manual will still be loaded"};
const char *ARG_OUTPUT_FILE[3] = {
        "-outputFile", 
        "Filename where the utility will output the QoS XML values", 
        "OPTIONAL: If not specified the output will be to the console"};
const char *ARG_PROFILE_PATH[3] = {
        "-profilePath", 
        "The fully qualified path of a QoS Profile"
        "\n\t\t e.g. QoSLibraryName::QoSProfileName", 
        "OPTIONAL: The default QoS for the -qosType will be returned"};
const char *ARG_QOS_TAG[3] = {
        "-qosTag", 
        "The XML tag name who QoS values you want to be fetched"
        "\n\t\t You can also select a subtag by separating it with a '/'"
        "\n\t\t e.g. datawriter_qos/history or participant_qos/property", 
        "REQUIRED: Allowed values = " 
        "\n\t\t\t \"datawriter_qos\", " 
        "\n\t\t\t \"datareader_qos\", " 
        "\n\t\t\t \"topic_qos\", " 
        "\n\t\t\t \"participant_qos\", " 
        "\n\t\t\t \"publisher_qos\", " 
        "\n\t\t\t \"subscriber_qos\""};
const char *ARG_TOPIC_NAME[3] = {
        "-topicName", 
        "Can be used with -qosTag = \"datawriter_qos\" | \"datareader_qos\" | \"topic_qos\"", 
        "OPTIONAL: The default value used with these types will be NULL"};

void print_help() {
    printf("RTI_XMLOutputUtility - (c) 2019 Copyright, Real-Time Innovations, Inc \n" 
            "A tool to visualize the final XML values provided by a Qos Profile \n\n");
    printf("Usage: %s [-OPTION] [VALUE] \n\n", 
            "[./]RTI_XMLOutputUtility[.exe]");
    printf("Options: \n");
    printf("%s \t %s \n \t\t %s \n\n", ARG_USER_FILE[0], ARG_USER_FILE[1], ARG_USER_FILE[2]);
    printf("%s \t %s \n \t\t %s \n\n", ARG_OUTPUT_FILE[0], ARG_OUTPUT_FILE[1], ARG_OUTPUT_FILE[2]);
    printf("%s \t %s \n \t\t %s \n\n", ARG_PROFILE_PATH[0], ARG_PROFILE_PATH[1], ARG_PROFILE_PATH[2]);
    printf("%s \t %s \n \t\t %s \n\n", ARG_QOS_TAG[0], ARG_QOS_TAG[1], ARG_QOS_TAG[2]);
    printf("%s \t %s \n \t\t %s \n\n", ARG_TOPIC_NAME[0], ARG_TOPIC_NAME[1], ARG_TOPIC_NAME[2]);
    printf("%s \t %s \n \t\t %s \n\n", ARG_HELP[0], ARG_HELP[1], ARG_HELP[2]);
}

int is_value(char *option_name, char *value) {
    if (value == NULL || value[0] == '-') {
        printf("No value provided for the option '%s'! \n", option_name);
        return 0;
    }
    return 1;
}

void CommandLineArguments_initialize(struct CommandLineArguments *cmd_args)
{
    cmd_args->output_file = NULL;
    cmd_args->qos_library = NULL;
    cmd_args->qos_profile = NULL;
    cmd_args->qos_type = NULL;
    cmd_args->query = NULL;
    cmd_args->topic_name = NULL;
    cmd_args->user_file = NULL;
}

void CommandLineArguments_finalize(struct CommandLineArguments *cmd_args)
{
    cmd_args->output_file = NULL;
    cmd_args->qos_library = NULL;
    cmd_args->qos_profile = NULL;
    free(cmd_args->qos_type);
    free(cmd_args->query);
    cmd_args->topic_name = NULL;
    cmd_args->user_file = NULL;
}

RTI_Retval parse_arguments(
        int argc, 
        char *argv[], 
        struct CommandLineArguments *output_values) 
{
    RTI_Retval result = ERROR;
    int i = 1;

    while (i < argc) {
        if (!strcmp(argv[i], ARG_HELP[0])) {
            print_help();
            goto done;
        } else if (!strcmp(argv[i], ARG_USER_FILE[0])) {
            if (is_value(argv[i], argv[i + 1])) {
                output_values->user_file = argv[i + 1];
            } else {
                goto done;
            }
        } else if (!strcmp(argv[i], ARG_OUTPUT_FILE[0])) {
            if (is_value(argv[i], argv[i + 1])) {
                output_values->output_file = argv[i + 1];
            } else {
                goto done;
            }
        } else if (!strcmp(argv[i], ARG_PROFILE_PATH[0])) {
            if (is_value(argv[i], argv[i + 1])) {
                char *token = NULL;
                int string_size = 0;

                token = strstr(argv[i + 1], SPLIT_STRING_PROFILE_PATH);
                if (token != NULL) {
                    string_size = (token - argv[i + 1]);

                    if (Common_allocate_string(
                                &output_values->qos_library, 
                                string_size) != OK) {
                        printf("Buffer allocation for '%s' field failed! \n", 
                                ARG_PROFILE_PATH[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->qos_library, 
                            argv[i + 1], 
                            string_size);
                    
                    string_size = 
                            strlen(argv[i + 1]) 
                            - strlen(output_values->qos_library) 
                            - strlen(SPLIT_STRING_PROFILE_PATH);

                    if (Common_allocate_string(
                                &output_values->qos_profile, 
                                string_size) != OK) {
                        printf("Buffer allocation for '%s' field failed! \n", 
                                ARG_PROFILE_PATH[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->qos_profile, 
                            token + strlen(SPLIT_STRING_PROFILE_PATH), 
                            string_size);
                } else {
                    printf("Parsing the value for option '%s' failed! \n", 
                            ARG_PROFILE_PATH[0]);
                    goto done;
                }
            } else {
                goto done;
            }
        } else if (!strcmp(argv[i], ARG_QOS_TAG[0])) {
            if (is_value(argv[i], argv[i + 1])) {
                char *token = NULL;
                int string_size = 0;

                token = strstr(argv[i + 1], SPLIT_STRING_QOS_TAG);
                if (token != NULL) {
                    string_size = (token - argv[i + 1]);

                    if (Common_allocate_string(
                                &output_values->qos_type, 
                                string_size) != OK) {
                        printf("Buffer allocation for '%s' field failed! \n", 
                                ARG_QOS_TAG[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->qos_type, 
                            argv[i + 1], 
                            string_size);
                    
                    string_size = 
                            strlen(argv[i + 1]) 
                            - strlen(output_values->qos_type) 
                            - strlen(SPLIT_STRING_QOS_TAG);

                    if (Common_allocate_string(
                                &output_values->query, 
                                string_size) != OK) {
                        printf("Buffer allocation for '%s' field failed! \n", 
                                ARG_QOS_TAG[0]);
                        goto done;
                    }
                    strncpy(
                            output_values->query, 
                            token + strlen(SPLIT_STRING_QOS_TAG), 
                            string_size);
                } else {
                    if (Common_allocate_string(
                                &output_values->qos_type, 
                                strlen(argv[i + 1])) != OK) {
                        printf("Buffer allocation for '%s' field failed! \n", 
                                ARG_QOS_TAG[0]);
                        goto done;
                    }
                    strcpy(output_values->qos_type, argv[i + 1]);
                }
            } else {
                goto done;
            }
        } else if (!strcmp(argv[i], ARG_TOPIC_NAME[0])) {
            if (is_value(argv[i], argv[i + 1])) {
                output_values->topic_name = argv[i + 1];
            } else {
                goto done;
            }
        } else {
            printf("Unknown option '%s'. Please run RTI_XMLOutputUtility with the -help option "
                    "to see the valid list of options. \n\n", argv[i]);
            goto done;
        }
        i += 2;
    }

    if (output_values->qos_type == NULL) {
        printf("ERROR: '%s' is a REQUIRED argument for the utility! \n", ARG_QOS_TAG[0]);
        goto done;
    }

    if (!(!strcmp(output_values->qos_type, "datawriter_qos") 
            || !strcmp(output_values->qos_type, "datareader_qos") 
            || !strcmp(output_values->qos_type, "topic_qos") 
            || !strcmp(output_values->qos_type, "participant_qos") 
            || !strcmp(output_values->qos_type, "publisher_qos") 
            || !strcmp(output_values->qos_type, "subscriber_qos"))) {
        printf("ERROR: \"%s\" doesn't match one of the expected values for '%s' option! " 
                "Please use the '%s' option for more information \n", 
                output_values->qos_type, 
                ARG_QOS_TAG[0], 
                ARG_HELP[0]);
        goto done;
    }

    result = OK;
done:
    return result;
}

void print_arguments(struct CommandLineArguments *values)
{
    printf("The parsed arguments are: \n");
    printf("%s \t %s \n", ARG_USER_FILE[0], values->user_file);
    printf("%s \t %s \n", ARG_OUTPUT_FILE[0], values->output_file);
    printf("%s \t %s %s \n", ARG_PROFILE_PATH[0], values->qos_library, values->qos_profile);
    printf("%s \t %s %s \n", ARG_QOS_TAG[0], values->qos_type, values->query);
    printf("%s \t %s \n", ARG_TOPIC_NAME[0], values->topic_name);
}