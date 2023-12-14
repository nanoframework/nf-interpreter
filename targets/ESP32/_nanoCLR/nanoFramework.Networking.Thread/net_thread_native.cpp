#include "net_thread_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Thread =
{
    "nanoFramework.Networking.Thread",
    0xEF27920E,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
