//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SERIALCFG_H
#define SERIALCFG_H

// define which serial driver the Wire Protocol will be using
#define SERIAL_DRIVER SD1

#if defined(FEATURE_TRACE_TO_STDIO)
// define which serial driver the stdio TRACE will be using
// (e.g. set "NF_FEATURE_TRACE_TO_STDIO": "4" in cmake-variants.json for the Traget)
//#define SERIAL_DRIVER_STDIO SD##FEATURE_TRACE_TO_STDIO
#endif

#endif /* SERIALCFG_H */

/** @} */
