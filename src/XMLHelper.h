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

#ifndef xmlhelper_h
#define xmlhelper_h

#include "Common.h"
#include "ndds/ndds_c.h"

#define INDENTATION_UNIT "    "

RTI_Retval XMLHelper_insert_enclosing_tag(
        char *qos_type, 
        struct RTIXMLSaveContext *context, 
        int closing);

RTI_Retval XMLHelper_dump_datawriter_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        char *topic_name, 
        struct RTIXMLSaveContext *context);

RTI_Retval XMLHelper_dump_datareader_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        char *topic_name, 
        struct RTIXMLSaveContext *context);

RTI_Retval XMLHelper_dump_topic_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        char *topic_name, 
        struct RTIXMLSaveContext *context);

RTI_Retval XMLHelper_dump_publisher_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context);

RTI_Retval XMLHelper_dump_subscriber_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context);

RTI_Retval XMLHelper_dump_participant_qos(
        DDS_DomainParticipantFactory *factory, 
        char *library_name, 
        char *profile_name, 
        struct RTIXMLSaveContext *context);

/* Forward declarations of RTIXMLUTILS */

struct RTIXMLUTILSObject;

RTIBool RTIXMLUTILSParser_parseString(
        struct RTIXMLUTILSObject **root,
        const char *xmlString);

void RTIXMLUTILSParser_freeDom(struct RTIXMLUTILSObject *element);

RTIBool RTIXMLUTILSObject_toStringWithParams(
        struct RTIXMLUTILSObject * self,
        char **outputBuffer,
        unsigned int *outputBufferLength,
        unsigned int outputBufferOffset,
        RTIBool prettyPrint);

struct RTIXMLUTILSObject* RTIXMLUTILSObject_getFirstChild(
        struct RTIXMLUTILSObject *self);

struct RTIXMLUTILSObject* RTIXMLUTILSObject_getNextSibling(
        struct RTIXMLUTILSObject *self);

const char* RTIXMLUTILSObject_getTagName(struct RTIXMLUTILSObject *self);

const char* RTIXMLUTILSObject_getText(struct RTIXMLUTILSObject *self);

int RTIXMLUTILSObject_getChildCount(struct RTIXMLUTILSObject *self);

struct RTIXMLUTILSObject* RTIXMLUTILSObject_getFirstChildWithTag(
        struct RTIXMLUTILSObject *self,
        const char *tagName);

/* Forward declaration end here */

void XMLHelper_print_node(
        struct RTIXMLUTILSObject *node, 
        unsigned int indent, 
        FILE *stream, 
        int is_root);

void XMLHelper_pretty_print(FILE *stream, char *string, char *query);

#endif /* xmlhelper_h */