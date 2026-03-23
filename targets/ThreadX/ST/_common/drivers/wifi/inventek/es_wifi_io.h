//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <network_options.h>

#if defined(CHIBIOS_HAL_REQUIRED)
#include "es_wifi_io_chibios.h"
#else
#include "es_wifi_io_stm.h"
#endif
