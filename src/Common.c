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
    RTI_Retval result = ERROR;

    /* calloc necessary along with length + 1 for correctly terminating C strings */
    *out_dest = (char *) calloc((length + 1), sizeof(char));
    if (*out_dest == NULL) {
        printf("Not enough memory for string allocation! \n");
        goto done;
    }
    result = OK;
done:
    return result;
}

RTI_Retval Common_allocate_and_copy_string(char **out_dest, char *src) {
    RTI_Retval result = ERROR;

    if (Common_allocate_string(out_dest, strlen(src)) != OK) {
        printf("Not enough memory for copying string '%s'! \n", src);
        goto done;
    }

    strcpy(*out_dest, src);
    result = OK;
done:
    return result;
}