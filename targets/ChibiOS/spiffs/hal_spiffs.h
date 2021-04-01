//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_SPIFFS_H_
#define HAL_SPIFFS_H_

#include <spiffs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPIFFS_SUCCESS          (0)
#define SPIFFS_ERROR            (-1)

extern spiffs fs;
extern spiffs_config spiffs_cfg;
extern bool spiffsFileSystemReady;

uint8_t hal_spiffs_config();

#ifdef __cplusplus
}
#endif

#endif // HAL_SPIFFS_H_
