/*
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
*/

SECTIONS
{
    .boot_clipboard_section (NOLOAD) :
    {
        . = ORIGIN(SECTION_FOR_BOOTCLIPBOARD);
        KEEP(*(.boot_clipboard*))
    } > SECTION_FOR_BOOTCLIPBOARD
}
