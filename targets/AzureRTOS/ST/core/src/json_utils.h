/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _JSON_UTILS_H
#define _JSON_UTILS_H

#include <stdbool.h>

#include "jsmn.h"

bool findJsonInt(const char* json, jsmntok_t* tokens, int tokens_count, const char* s, int* value);
bool findJsonString(const char* json, jsmntok_t* tokens, int tokens_count, const char* s, char* value);

#endif