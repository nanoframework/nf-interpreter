//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

void* platform_malloc(size_t size) {
    return malloc(size);
}

void  platform_free(void*  ptr) {
    free(ptr);
}

void* platform_realloc(void*  ptr, size_t size) {
    return ptr;
}
