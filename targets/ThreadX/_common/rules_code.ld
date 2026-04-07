/*
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
*/

__nanoImage_start__     = ORIGIN(flash0);
__nanoImage_size__      = LENGTH(flash0);
__nanoImage_end__       = __nanoImage_start__ + __nanoImage_size__;
__nanoConfig_start__    = ORIGIN(config);
__nanoConfig_size__     = LENGTH(config);
__nanoConfig_end__      = __nanoConfig_start__ + __nanoConfig_size__;
__deployment_start__    = ORIGIN(deployment);
__deployment_size__     = LENGTH(deployment);
__deployment_end__      = __deployment_start__ + __deployment_size__;
