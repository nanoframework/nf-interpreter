//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef PLATFORM_POSIX_SELECTOR_H
#define PLATFORM_POSIX_SELECTOR_H

#define PLATFORM_POSIX_HOST 1

#if NANOCLR_HOST_APPLE
#define PLATFORM_APPLE_HOST 1
#endif

#if NANOCLR_HOST_LINUX
#define PLATFORM_LINUX_HOST 1
#endif

#endif // PLATFORM_POSIX_SELECTOR_H
