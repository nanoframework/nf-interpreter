//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Compatibility shim for NimBLE modlog numeric-level macros with GCC 15+.
//
// NimBLE defines log-level constants as integers (e.g. DEBUG=1, INFO=2, ...).
// When MODLOG_DFLT(DEBUG, ...) expands, DEBUG is resolved to 1 before the
// ## token-paste in modlog.h fires, producing MODLOG_1 which is not defined
// in the ESP-IDF NimBLE port's modlog.h. GCC 15 promotes that implicit-
// function-declaration from a warning to an error. The definitions below
// provide silent no-op fallbacks for every numeric variant that modlog.h
// leaves undefined, without modifying any ESP-IDF source files.

#ifndef NIMBLE_MODLOG_COMPAT_H
#define NIMBLE_MODLOG_COMPAT_H

#ifndef MODLOG_1
// NOLINTBEGIN
#define MODLOG_1(ml_mod_, ...) ((void)0)
// NOLINTEND
#endif

#ifndef MODLOG_2
// NOLINTBEGIN
#define MODLOG_2(ml_mod_, ...) ((void)0)
// NOLINTEND
#endif

#ifndef MODLOG_3
// NOLINTBEGIN
#define MODLOG_3(ml_mod_, ...) ((void)0)
// NOLINTEND
#endif

#ifndef MODLOG_4
// NOLINTBEGIN
#define MODLOG_4(ml_mod_, ...) ((void)0)
// NOLINTEND
#endif

#ifndef MODLOG_5
// NOLINTBEGIN
#define MODLOG_5(ml_mod_, ...) ((void)0)
// NOLINTEND
#endif

#endif // NIMBLE_MODLOG_COMPAT_H
