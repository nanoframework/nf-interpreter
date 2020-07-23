/*
//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
*/

__nanoImage_start__     = ORIGIN(FLASH);
__nanoImage_size__      = LENGTH(FLASH);
__nanoImage_end__       = __nanoImage_start__ + __nanoImage_size__;
__nanoConfig_start__    = ORIGIN(CONFIG);
__nanoConfig_size__     = LENGTH(CONFIG);
__nanoConfig_end__      = __nanoConfig_start__ + __nanoConfig_size__;
__deployment_start__    = ORIGIN(DEPLOYMENT);
__deployment_size__     = LENGTH(DEPLOYMENT);
__deployment_end__      = __deployment_start__ + __deployment_size__;
