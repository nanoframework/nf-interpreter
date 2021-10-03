/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_utils.h"

bool findJsonInt(const char* json, jsmntok_t* tokens, int tokens_count, const char* s, int* value)
{
    for (int i = 0; i < tokens_count - 1; i++)
    {
        if (tokens[i].type == JSMN_STRING && tokens[i + 1].type == JSMN_PRIMITIVE)
        {
            if (((int)strlen(s) == tokens[i].end - tokens[i].start) &&
                (strncmp(json + tokens[i].start, s, tokens[i].end - tokens[i].start) == 0))
            {
                *value = atoi(json + tokens[i + 1].start);

                return true;
            }
        }
    }

    return false;
}

bool findJsonString(const char* json, jsmntok_t* tokens, int tokens_count, const char* s, char* value)
{
    int key_len;
    int value_len;
    for (int i = 0; i < tokens_count - 1; i++)
    {
        if (tokens[i].type == JSMN_STRING && tokens[i + 1].type == JSMN_STRING)
        {
            key_len = tokens[i].end - tokens[i].start;
            if (((int)strlen(s) == key_len) && (strncmp(json + tokens[i].start, s, key_len) == 0))
            {
                value_len = tokens[i + 1].end - tokens[i + 1].start;
                strncpy(value, json + tokens[i + 1].start, value_len);
                value[value_len] = 0;

                return true;
            }
        }
    }

    return false;
}
