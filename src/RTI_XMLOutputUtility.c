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
#include "Common.h"
#include "CommandLineArgumentParser.h"
#include "XMLHelper.h"

void RTI_XMLOutputUtility_invoke_formatting_helper(char *file_name) 
{
    char python_command[200] = {0};
    sprintf(python_command, "python formatter.py %s", file_name);
    system(python_command);
    printf("Formatted file %s saved successfully \n", file_name);
}

RTI_Retval RTI_XMLOutputUtility_process_arguments(
        DDS_DomainParticipantFactory *factory,
        struct CommandLineArguments *cmd_args, 
        struct RTIXMLSaveContext *xml_save_context) 
{
    RTI_Retval result = ERROR;

    if (!strcmp("datareader_qos", cmd_args->qos_type)) {
        if (XMLHelper_dump_datareader_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                cmd_args->topic_name, 
                xml_save_context) != OK) {
            goto done;
        }
    } else if(!strcmp("datawriter_qos", cmd_args->qos_type)) {
        if (XMLHelper_dump_datawriter_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                cmd_args->topic_name, 
                xml_save_context) != OK) {
            goto done;
        }
    } else if(!strcmp("topic_qos", cmd_args->qos_type)) {
        if (XMLHelper_dump_topic_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                cmd_args->topic_name, 
                xml_save_context) != OK) {
            goto done;
        }
    } else if(!strcmp("publisher_qos", cmd_args->qos_type)) {
        if (XMLHelper_dump_publisher_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                xml_save_context) != OK) {
            goto done;
        }
    } else if(!strcmp("subscriber_qos", cmd_args->qos_type)) {
        if (XMLHelper_dump_subscriber_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                xml_save_context) != OK) {
            goto done;
        }
    } else if(!strcmp("participant_qos", cmd_args->qos_type)) {
        if (XMLHelper_dump_participant_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                xml_save_context) != OK) {
            goto done;
        }
    }
    result = OK;
done:
    return result;
}

int main(int argc, char *argv[])
{
    struct RTIXMLSaveContext xml_save_context = RTIXMLSaveContext_INITIALIZER;
    DDS_DomainParticipantFactory *factory = DDS_TheParticipantFactory;
    struct DDS_DomainParticipantFactoryQos dpf_qos = DDS_DomainParticipantFactoryQos_INITIALIZER;
    struct CommandLineArguments cmd_args;
    char *user_qos_profile_file = NULL;
    RTI_Retval result = ERROR;

    CommandLineArguments_initialize(&cmd_args);
    result = parse_arguments(argc, argv, &cmd_args);
    print_arguments(&cmd_args);

    if (result == ERROR) {
        goto done;
    }

    if (cmd_args.user_file != NULL) {
        if (Common_allocate_string(
                    &user_qos_profile_file, 
                    strlen("file://") + strlen(cmd_args.user_file)) != OK) {
            printf("String allocation for file name '%s' failed! \n", 
                    cmd_args.user_file);
            goto done;
        }
        sprintf(
                user_qos_profile_file, 
                "file://%s", 
                cmd_args.user_file);

        DDS_StringSeq_ensure_length(&dpf_qos.profile.url_profile, 1, 1);
        *DDS_StringSeq_get_reference(&dpf_qos.profile.url_profile, 0) 
                = DDS_String_dup(user_qos_profile_file);
        DDS_DomainParticipantFactory_set_qos(factory, &dpf_qos);
    }

    xml_save_context.depth = 1;
    if (DDS_DomainParticipantFactory_load_profiles(factory) 
            != DDS_RETCODE_OK) {
        printf("Failed to load the XML configuration files \n");
        goto done;
    }

    while (1) {
        if (RTI_XMLOutputUtility_process_arguments(
                factory, 
                &cmd_args, 
                &xml_save_context) != OK) {
            printf("The processing of arguments failed! \n");
            goto done;
        }

        if (xml_save_context.sout == NULL) {
            if (Common_allocate_string(
                    &xml_save_context.sout, 
                    xml_save_context.outputStringLength) != OK) {
                printf("Buffer allocation for XML output failed! \n");
                goto done;
            }
            xml_save_context.ssize = xml_save_context.outputStringLength;
            xml_save_context.outputStringLength = 0;
        } else {
            break;
        }
    }

    if (cmd_args.output_file != NULL) {
        xml_save_context.fout = fopen(cmd_args.output_file, "w");
        if (xml_save_context.fout == NULL) {
            printf("Failed to open file %s for writing! \n", cmd_args.output_file);
            goto done;
        }
        XMLHelper_pretty_print(
                xml_save_context.fout, 
                xml_save_context.sout, 
                cmd_args.query);
    } else {
        XMLHelper_pretty_print(
                stdout, 
                xml_save_context.sout, 
                cmd_args.query);
    }

    result = OK;
done:
    free(user_qos_profile_file);
    if (xml_save_context.fout != NULL) {
        printf("File '%s' save successfully! \n", cmd_args.output_file);
        fclose(xml_save_context.fout);
    }

    if (xml_save_context.sout != NULL) {
        free(xml_save_context.sout);
    }

    CommandLineArguments_finalize(&cmd_args);
    DDS_DomainParticipantFactoryQos_finalize(&dpf_qos);

    return result;
}