//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Stub nf_config.h for the Windows/netcore target.
// Embedded targets generate this file at build time via the Kconfig pipeline.
// The Windows emulator does not use Kconfig, so every CONFIG_* symbol that
// is consumed by shared source files is defined here manually, matching the
// feature set that was previously implied by nf.props and target_os.h.

#ifndef NF_CONFIG_H
#define NF_CONFIG_H

// ── RTOS identity ────────────────────────────────────────────────────────────
// Required by nanoHAL_Boot.h / nanoHAL_Capabilites.h to confirm inclusion.
#define CONFIG_RTOS "WIN32"

// ── Target hardware ──────────────────────────────────────────────────────────
// The Windows virtual device has no nanoBooter.
#define CONFIG_NF_TARGET_HAS_NANOBOOTER 0

// ── Build options ────────────────────────────────────────────────────────────
// Windows uses native double-precision floating point.
#define CONFIG_NF_DOUBLE_PRECISION_FP 1
// Full math library — do not exclude complex math functions.
#define CONFIG_NF_FEATURE_LIGHT_MATH 0
// String conversion to any numeric base.
#define CONFIG_NF_SUPPORT_ANY_BASE_CONVERSION 1

// ── nanoFramework features ────────────────────────────────────────────────────
// Managed application debugging (debug transport is always present).
#define CONFIG_NF_FEATURE_DEBUGGER 1
// System.Reflection API (was NANOCLR_REFLECTION=TRUE in nf.props).
#define CONFIG_NF_FEATURE_SUPPORT_REFLECTION 1
// Binary serialization support.
#define CONFIG_NF_FEATURE_BINARY_SERIALIZATION 1

// ── nanoFramework APIs ────────────────────────────────────────────────────────
// nanoFramework.System.Collections — enables CLR_RT_HeapBlock_Stack/Queue
// (was NANOCLR_SYSTEM_COLLECTIONS=TRUE in nf.props).
#define CONFIG_API_NANOFRAMEWORK_SYSTEM_COLLECTIONS 1

#endif // NF_CONFIG_H
