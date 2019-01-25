//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_WINDOWS_STORAGE_H_
#define _TARGET_WINDOWS_STORAGE_H_ 1

#include <ch.h>
#include <cmsis_os.h>
#include <hal.h>
#include <ff.h>

#define POLLING_INTERVAL                10
#define POLLING_DELAY                   10

// void RemoveHandler(eventid_t id);
// void InsertHandler(eventid_t id);

#ifdef __cplusplus
extern "C" {
#endif

void Target_FileSystemInit(void);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_WINDOWS_STORAGE_H_
