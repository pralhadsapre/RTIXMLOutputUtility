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

RTI_Retval Common_allocate_string(char **out_dest, size_t length) {
    RTI_Retval result = RTI_ERROR;

    *out_dest = DDS_String_alloc(length);
    if (*out_dest == NULL) {
        printf("Not enough memory for string allocation! \n");
        goto done;
    }
    result = RTI_OK;
done:
    return result;
}

RTI_Retval Common_allocate_and_copy_string(char **out_dest, const char *src) {
    RTI_Retval result = RTI_ERROR;

    *out_dest = DDS_String_dup(src);
    if (out_dest == NULL) {
        printf("Not enough memory for copying string '%s'! \n", src);
        goto done;
    }

    result = RTI_OK;
done:
    return result;
}