//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// RP2350 startup objects can reference core1 hooks even in single-core builds.
// Provide no-op stubs so the linker can resolve them when core1 is not used.

void __c1_cpu_init(void)
{
}

void __c1_early_init(void)
{
}

void __c1_late_init(void)
{
}

void c1_main(void)
{
    while (1)
    {
    }
}

void __c1_default_exit(void)
{
    while (1)
    {
    }
}
