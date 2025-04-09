 SECTIONS
{
    .boot_clipboard_section (NOLOAD) :
    {
        . = ORIGIN(SECTION_FOR_BOOTCLIPBOARD);
        KEEP(*(.boot_clipboard*))
    } > SECTION_FOR_BOOTCLIPBOARD
}
