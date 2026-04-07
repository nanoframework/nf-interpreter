//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGETPAL_H
#define TARGETPAL_H

// Gets the GPIO port according to a pin number
#define GPIO_PORT(pin) ((GPIO_Port_TypeDef)(pin / 16))

#endif // TARGETPAL_H
