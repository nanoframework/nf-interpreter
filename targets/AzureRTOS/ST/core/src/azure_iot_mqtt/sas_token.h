/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _SAS_TOKEN_H
#define _SAS_TOKEN_H

#include <stdbool.h>

bool create_sas_token(char* key,
    unsigned int key_size,
    char* hostname,
    char* device_id,
    unsigned long valid_until,
    char* output,
    unsigned int output_size);

bool create_dps_sas_token(char *key, unsigned int key_size, char *id_scope,
                          char *registration_id, unsigned long valid_until,
    char* output,
    unsigned int output_size);

#endif // _SAS_TOKEN_H
