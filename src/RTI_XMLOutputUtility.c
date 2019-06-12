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
#include "XMLHelper.h"

DDS_Boolean RTI_XMLOutputUtility_process_arguments(
        struct CommandLineArguments *cmd_args, 
        struct RTIXMLSaveContext *xml_save_context) 
{
    DDS_DomainParticipantFactory *factory = DDS_TheParticipantFactory;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    if (strcmp("datareader_qos", cmd_args->qos_type) == 0) {
        if (XMLHelper_dump_datareader_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                cmd_args->topic_name, 
                xml_save_context) != DDS_BOOLEAN_TRUE) {
            goto done;
        }
    } else if(strcmp("datawriter_qos", cmd_args->qos_type) == 0) {
        if (XMLHelper_dump_datawriter_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                cmd_args->topic_name, 
                xml_save_context) != DDS_BOOLEAN_TRUE) {
            goto done;
        }
    } else if(strcmp("topic_qos", cmd_args->qos_type) == 0) {
        if (XMLHelper_dump_topic_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                cmd_args->topic_name, 
                xml_save_context) != DDS_BOOLEAN_TRUE) {
            goto done;
        }
    } else if(strcmp("publisher_qos", cmd_args->qos_type) == 0) {
        if (XMLHelper_dump_publisher_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                xml_save_context) != DDS_BOOLEAN_TRUE) {
            goto done;
        }
    } else if(strcmp("subscriber_qos", cmd_args->qos_type) == 0) {
        if (XMLHelper_dump_subscriber_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                xml_save_context) != DDS_BOOLEAN_TRUE) {
            goto done;
        }
    } else if(strcmp("participant_qos", cmd_args->qos_type) == 0) {
        if (XMLHelper_dump_participant_qos(
                factory, 
                cmd_args->qos_library, 
                cmd_args->qos_profile, 
                xml_save_context) != DDS_BOOLEAN_TRUE) {
            goto done;
        }
    }
    result = DDS_BOOLEAN_TRUE;
done:
    return result;
}

int main(int argc, char *argv[])
{
    struct RTIXMLSaveContext xml_save_context = RTIXMLSaveContext_INITIALIZER;
    DDS_DomainParticipantFactory *factory = DDS_TheParticipantFactory;
    struct DDS_XMLObject *root = NULL, *default_profile = NULL;
    struct DDS_DomainParticipantFactoryQos dpf_qos = DDS_DomainParticipantFactoryQos_INITIALIZER;
    struct CommandLineArguments cmd_args;
    char *user_qos_profile_file = NULL;
    DDS_Boolean result = DDS_BOOLEAN_FALSE;

    CommandLineArguments_initialize(&cmd_args);
    result = CommandLineArgumentParser_parse_arguments(argc, argv, &cmd_args);
    CommandLineArgumentParser_print_arguments(&cmd_args);

    if (result == DDS_BOOLEAN_FALSE) {
        goto done;
    }

    if (cmd_args.user_file != NULL) {
        if (Common_allocate_string(
                    &user_qos_profile_file, 
                    strlen("file://") + strlen(cmd_args.user_file)) != DDS_BOOLEAN_TRUE) {
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

    if (DDS_DomainParticipantFactory_load_profiles(factory) 
            != DDS_RETCODE_OK) {
        printf("Failed to load the XML configuration files \n");
        goto done;
    }

    if (cmd_args.qos_library == NULL || cmd_args.qos_profile == NULL) {
        root = DDS_QosProvider_get_xml_root(
                DDS_DomainParticipantFactory_get_qos_providerI(factory));
        if (root != NULL) {
            default_profile = (struct DDS_XMLObject *) DDS_XMLDds_get_default_qos_profile(
                    (struct DDS_XMLDds *) root);
            if (default_profile != NULL) {
                cmd_args.qos_library = 
                        DDS_String_dup(
                                DDS_XMLObject_get_name(
                                        DDS_XMLObject_get_parent(default_profile)));
                cmd_args.qos_profile = 
                        DDS_String_dup(
                                DDS_XMLObject_get_name(default_profile));
                printf("The default <qos_profile> detected was '%s::%s'! \n", 
                        cmd_args.qos_library, 
                        cmd_args.qos_profile);
            } else {
                printf("There is no <qos_profile> marked with is_default_qos=\"true\", "
                        "getting the default QoS values for <%s> now! \n", 
                        cmd_args.qos_type);
            }
        } else {
            printf("No QoS Profiles found! \n");
            goto done;
        }
    }

    xml_save_context.depth = 1;
    while (1) {
        if (RTI_XMLOutputUtility_process_arguments(
                &cmd_args, 
                &xml_save_context) != DDS_BOOLEAN_TRUE) {
            printf("The processing of arguments failed! \n");
            goto done;
        }

        if (xml_save_context.sout == NULL) {
            if (Common_allocate_string(
                    &xml_save_context.sout, 
                    xml_save_context.outputStringLength) != DDS_BOOLEAN_TRUE) {
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
    } else {
        xml_save_context.fout = stdout;
    }

    XMLHelper_pretty_print(
            xml_save_context.fout, 
            xml_save_context.sout, 
            cmd_args.query);

    result = DDS_BOOLEAN_TRUE;
done:
    DDS_String_free(user_qos_profile_file);

    if (cmd_args.output_file != NULL) {
        printf("File '%s' save successfully! \n", cmd_args.output_file);
        fclose(xml_save_context.fout);
    }

    if (xml_save_context.sout != NULL) {
        DDS_String_free(xml_save_context.sout);
    }

    CommandLineArguments_finalize(&cmd_args);
    DDS_DomainParticipantFactoryQos_finalize(&dpf_qos);

    return result;
}