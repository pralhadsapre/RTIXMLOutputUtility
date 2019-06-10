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
#include "XMLHelper.h"

void XMLHelper_print_node(
        struct RTIXMLUTILSObject *node, 
        unsigned int indent, 
        FILE *stream, 
        int is_root) 
{
    int i = 0, has_children = 0;
    const char *text = NULL;
    while (node) {
        has_children = RTIXMLUTILSObject_getChildCount(node) > 0 ? 1 : 0;

        for (i = 0; i < indent; i++) {
            fprintf(stream, "%s", INDENTATION_UNIT);
        }
        fprintf(stream, "<%s>", RTIXMLUTILSObject_getTagName(node));
        if (has_children) {
            fprintf(stream, "\n");
        }

        text = RTIXMLUTILSObject_getText(node);
        if (strlen(text) > 0) {
            for (i = 0; i < indent + 1 && has_children; i++) {
                fprintf(stream, "%s", INDENTATION_UNIT);
            }
            fprintf(stream, "%s", text);
            if (has_children) {
                fprintf(stream, "\n");
            }
        }

        XMLHelper_print_node(
                RTIXMLUTILSObject_getFirstChild(node), 
                indent + 1, 
                stream, 
                0);

        for (i = 0; i < indent && has_children; i++) {
            fprintf(stream, "%s", INDENTATION_UNIT);
        }
        fprintf(stream, "</%s>\n", RTIXMLUTILSObject_getTagName(node));

        if (!is_root) {
            node = RTIXMLUTILSObject_getNextSibling(node);
        } else {
            break;
        }
    }
}

void XMLHelper_pretty_print(FILE *stream, char *string, char *query) 
{
    struct RTIXMLUTILSObject *dom_root = NULL, *query_root = NULL;

    RTIXMLUTILSParser_parseString(&dom_root, string);
    if (query != NULL) {
        query_root = RTIXMLUTILSObject_getFirstChildWithTag(dom_root, query);
        if (query_root == NULL) {
            printf("The queried subtag '%s' couldn't be found! \n", query);
        } else {
            XMLHelper_print_node(query_root, 0, stream, 1);
        }
    } else {
        XMLHelper_print_node(dom_root, 0, stream, 1);
    }

    RTIXMLUTILSParser_freeDom(dom_root);
}

RTI_Retval XMLHelper_insert_enclosing_tag(
        char *qos_type, 
        struct RTIXMLSaveContext *context, 
        int closing)
{
    RTI_Retval result = ERROR;
    const char *closing_format = "</%s>\n\0";
    const char *opening_format = "<%s>\n";
    const int closing_format_length = 5;
    const int opening_format_length = 3;
    int format_length = 0;
    char *format = NULL;

    if (closing) {
        format_length = closing_format_length;
        format = (char *) closing_format;
    } else {
        format_length = opening_format_length;
        format = (char *) opening_format;
    }

    if (context->fout != NULL) {
        fprintf(context->fout, format, qos_type);
        context->outputStringLength += (strlen(qos_type) + format_length);
    } else if (context->sout != NULL) {
        if (context->ssize >= (context->outputStringLength + strlen(qos_type) + format_length)) {
            char *current_position = context->sout;
            current_position += context->outputStringLength;
            sprintf(current_position, format, qos_type);
            context->outputStringLength += (strlen(qos_type) + format_length);
        } else {
            goto done;
        }
    } else {
        context->outputStringLength += (strlen(qos_type) + format_length);
    }

    result = OK;
done:
    return result;
}

/* 
 * Useful when when no library_name or profile_name is specified and we want 
 * to identify a <qos_profile> with a is_default_qos="true" value.
 */
DDS_DomainParticipant* XMLHelper_create_dummy_participant(
        DDS_DomainParticipantFactory *factory) 
{
    DDS_DomainParticipant *dummy_participant = NULL;
    dummy_participant = DDS_DomainParticipantFactory_create_participant(
            factory, 
            0, /* Domain ID */
            &DDS_PARTICIPANT_QOS_DEFAULT, 
            NULL /* Listener */, 
            DDS_STATUS_MASK_NONE);
    if (dummy_participant == NULL) {
        printf("Failed to create the dummy participant! \n");
    }
    return dummy_participant;
}

RTI_Retval XMLHelper_dump_datawriter_qos(
        DDS_DomainParticipantFactory *factory, 
        DDS_DomainParticipant *dummy_participant, 
        char *library_name, 
        char *profile_name, 
        char *topic_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_DataWriterQos datawriter_qos = DDS_DataWriterQos_INITIALIZER;
    RTI_Retval result = ERROR;

    XMLHelper_insert_enclosing_tag("datawriter_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (dummy_participant == NULL) {
            goto done;
        } else {
            if (DDS_DomainParticipant_get_default_datawriter_qos_w_topic_name(
                        dummy_participant, 
                        &datawriter_qos, 
                        topic_name) != DDS_RETCODE_OK) {
                printf("Failed to fetch default <datawriter_qos> values for topic name %s! \n",  
                        topic_name);
                goto done;
            }
        }
    } else {
        if (DDS_DomainParticipantFactory_get_datawriter_qos_from_profile_w_topic_name(
                    factory, 
                    &datawriter_qos, 
                    library_name, 
                    profile_name, 
                    topic_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <datawriter_qos> values under %s::%s for topic name %s! \n", 
                    library_name, 
                    profile_name, 
                    topic_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "publication_name", 
            &datawriter_qos.publication_name, 
            NULL, 
            context);
    DDS_DataWriterQos_save(&datawriter_qos, NULL, context);
    XMLHelper_insert_enclosing_tag("datawriter_qos", context, 1);

    result = OK;
done:
    if (DDS_DataWriterQos_finalize(&datawriter_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <datawriter_qos>! \n");
        result = ERROR;
    }
    return result;
}

RTI_Retval XMLHelper_dump_datareader_qos(
        DDS_DomainParticipantFactory *factory, 
        DDS_DomainParticipant *dummy_participant, 
        char *library_name, 
        char *profile_name, 
        char *topic_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_DataReaderQos datareader_qos = DDS_DataReaderQos_INITIALIZER;
    RTI_Retval result = ERROR;

    XMLHelper_insert_enclosing_tag("datareader_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (dummy_participant == NULL) {
            goto done;
        } else {
            if (DDS_DomainParticipant_get_default_datareader_qos_w_topic_name(
                        dummy_participant, 
                        &datareader_qos, 
                        topic_name) != DDS_RETCODE_OK) {
                printf("Failed to fetch default <datareader_qos> values for topic name %s! \n",  
                        topic_name);
                goto done;
            }
        }
    } else {
        if (DDS_DomainParticipantFactory_get_datareader_qos_from_profile_w_topic_name(
                    factory, 
                    &datareader_qos, 
                    library_name, 
                    profile_name, 
                    topic_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <datareader_qos> values under %s::%s for topic name %s! \n", 
                    library_name, 
                    profile_name, 
                    topic_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "subscription_name", 
            &datareader_qos.subscription_name, 
            NULL, 
            context);
    DDS_DataReaderQos_save(&datareader_qos, NULL, context);
    XMLHelper_insert_enclosing_tag("datareader_qos", context, 1);

    result = OK;
done:
    if (DDS_DataReaderQos_finalize(&datareader_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <datareader_qos>! \n");
        result = ERROR;
    }
    return result;
}

RTI_Retval XMLHelper_dump_topic_qos(
        DDS_DomainParticipantFactory *factory, 
        DDS_DomainParticipant *dummy_participant, 
        char *library_name, 
        char *profile_name, 
        char *topic_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_TopicQos topic_qos = DDS_TopicQos_INITIALIZER;
    RTI_Retval result = ERROR;

    XMLHelper_insert_enclosing_tag("topic_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        dummy_participant = XMLHelper_create_dummy_participant(factory);
        if (dummy_participant == NULL) {
            goto done;
        } else {
            if (DDS_DomainParticipant_get_default_topic_qos_w_topic_name(
                        dummy_participant, 
                        &topic_qos, 
                        topic_name) != DDS_RETCODE_OK) {
                printf("Failed to fetch default <topic_qos> values for topic name %s! \n",  
                        topic_name);
                goto done;
            }
        }
    } else {
        if (DDS_DomainParticipantFactory_get_topic_qos_from_profile_w_topic_name(
                    factory, 
                    &topic_qos, 
                    library_name, 
                    profile_name, 
                    topic_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <topic_qos> values under %s::%s for topic name %s! \n", 
                    library_name, 
                    profile_name, 
                    topic_name);
            goto done;
        }
    }
    DDS_TopicQos_save(&topic_qos, NULL, context);
    XMLHelper_insert_enclosing_tag("topic_qos", context, 1);

    result = OK;
done:
    if (DDS_TopicQos_finalize(&topic_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <topic_qos>! \n");
        result = ERROR;
    }
    return result;
}

RTI_Retval XMLHelper_dump_publisher_qos(
        DDS_DomainParticipantFactory *factory, 
        DDS_DomainParticipant *dummy_participant, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_PublisherQos publisher_qos = DDS_PublisherQos_INITIALIZER;
    RTI_Retval result = ERROR;

    XMLHelper_insert_enclosing_tag("publisher_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (dummy_participant == NULL) {
            goto done;
        } else {
            if (DDS_DomainParticipant_get_default_publisher_qos(
                        dummy_participant, 
                        &publisher_qos) != DDS_RETCODE_OK) {
                printf("Failed to fetch default <publisher_qos> values! \n");
                goto done;
            }
        }
    } else {
        if (DDS_DomainParticipantFactory_get_publisher_qos_from_profile(
                    factory, 
                    &publisher_qos, 
                    library_name, 
                    profile_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <publisher_qos> values under %s::%s! \n", 
                    library_name, 
                    profile_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "publisher_name", 
            &publisher_qos.publisher_name, 
            NULL, 
            context);
    DDS_PublisherQos_save(&publisher_qos, NULL, context);
    XMLHelper_insert_enclosing_tag("publisher_qos", context, 1);

    result = OK;
done:
    if (DDS_PublisherQos_finalize(&publisher_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <publisher_qos>! \n");
        result = ERROR;
    }
    return result;
}

RTI_Retval XMLHelper_dump_subscriber_qos(
        DDS_DomainParticipantFactory *factory, 
        DDS_DomainParticipant *dummy_participant, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_SubscriberQos subscriber_qos = DDS_SubscriberQos_INITIALIZER;
    RTI_Retval result = ERROR;

    XMLHelper_insert_enclosing_tag("subscriber_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        dummy_participant = XMLHelper_create_dummy_participant(factory);
        if (dummy_participant == NULL) {
            goto done;
        } else {
            if (DDS_DomainParticipant_get_default_subscriber_qos(
                        dummy_participant, 
                        &subscriber_qos) != DDS_RETCODE_OK) {
                printf("Failed to fetch default <subscriber_qos> values! \n");
                goto done;
            }
        }
    } else {
        if (DDS_DomainParticipantFactory_get_subscriber_qos_from_profile(
                    factory, 
                    &subscriber_qos, 
                    library_name, 
                    profile_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <subscriber_qos> values under %s::%s! \n", 
                    library_name, 
                    profile_name);
            goto done;
        }
    }
    DDS_EntityNameQosPolicy_save(
            "subscriber_name", 
            &subscriber_qos.subscriber_name, 
            NULL, 
            context);
    DDS_SubscriberQos_save(&subscriber_qos, NULL, context);
    XMLHelper_insert_enclosing_tag("subscriber_qos", context, 1);

    result = OK;
done:
    if (DDS_SubscriberQos_finalize(&subscriber_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <subscriber_qos>! \n");
        result = ERROR;
    }
    return result;
}

RTI_Retval XMLHelper_dump_participant_qos(
        DDS_DomainParticipantFactory *factory, 
        DDS_DomainParticipant *dummy_participant, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context) 
{
    struct DDS_DomainParticipantQos participant_qos = DDS_DomainParticipantQos_INITIALIZER;
    RTI_Retval result = ERROR;

    XMLHelper_insert_enclosing_tag("participant_qos", context, 0);
    if (library_name == NULL || profile_name == NULL) {
        if (dummy_participant == NULL) {
            goto done;
        } else {
            if (DDS_DomainParticipant_get_qos(
                        dummy_participant, 
                        &participant_qos) != DDS_RETCODE_OK) {
                printf("Failed to fetch default <participant_qos> values! \n");
                goto done;
            }
        }
    } else {
        if (DDS_DomainParticipantFactory_get_participant_qos_from_profile(
                    factory, 
                    &participant_qos, 
                    library_name, 
                    profile_name) != DDS_RETCODE_OK) {
            printf("Failed to fetch <participant_qos> values under %s::%s! \n", 
                    library_name, 
                    profile_name);
            goto done;
        }
    }
    DDS_DomainParticipantQos_save(&participant_qos, NULL, context);
    XMLHelper_insert_enclosing_tag("participant_qos", context, 1);

    result = OK;
done:
    if (DDS_DomainParticipantQos_finalize(&participant_qos) != DDS_RETCODE_OK) {
        printf("Failed to delete the <participant_qos>! \n");
        result = ERROR;
    }
    return result;
}