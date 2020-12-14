/*
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
*/

/* Highest address of the user mode stack */
_estack = 0x20018000;    /* end of RAM */

/* Set the RAM segment used end for threadx */
__RAM_segment_used_end__ = 0; 

/* Specify the memory areas */
MEMORY
{
  flash0     (rx)  : ORIGIN = 0x08008800, LENGTH = 186K         /* space reserved for nanoCLR */
  deployment (rx)  : ORIGIN = 0x08036800, LENGTH = 1M-186K-34K  /* space reserved for application deployment */
  config     (rw)  : ORIGIN = 0x080FF800, LENGTH = 2k           /* space reserved for configuration block */
  ram0       (xrw) : ORIGIN = 0x20000030, LENGTH = 96K-48
  ram2       (xrw) : ORIGIN = 0x10000000, LENGTH = 32K          /* managed heap */
  bootclpbrd (wx)  : ORIGIN = 0x20000000, LENGTH = 48           /* boot clipboard area */
}

/* RAM region to be used for the boot clipboard.*/
REGION_ALIAS("SECTION_FOR_BOOTCLIPBOARD", bootclpbrd);

/* RAM region to be used for the nanoFramework CLR managed heap.*/
REGION_ALIAS("CLR_MANAGED_HEAP_RAM", ram2);

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into flash0 */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >flash0

  /* The program code and other data goes into flash0 */
  .text :
  {
    . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >flash0

  /* Constant data goes into flash0 */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >flash0

  .ARM.extab   : 
  { 
  . = ALIGN(8);
  *(.ARM.extab* .gnu.linkonce.armextab.*)
  . = ALIGN(8);
  } >flash0
  .ARM : {
	. = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
	. = ALIGN(8);
  } >flash0

  .preinit_array     :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
	. = ALIGN(8);
  } >flash0
  
  .init_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
	. = ALIGN(8);
  } >flash0
  .fini_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
	. = ALIGN(8);
  } >flash0

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into ram0, load LMA copy after code */
  .data : 
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >ram0 AT> flash0

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >ram0

  /* User_heap_stack section, used to check that there is enough ram0 left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    __heap_base__ = .;
    . += __crt_heap_size__;
    . = ALIGN(8);
    __heap_end__ = .;
    __main_stack_base__ = .;
    . += __main_stack_size__;
    . = ALIGN(8);
    __main_stack_end__ = .;
    . = ALIGN(8);
  } >ram0

  /* nanoFramework CLR managed heap section at the specified RAM section.*/
  .clr_managed_heap (NOLOAD) :
  {
      . = ALIGN(8);
      __clr_managed_heap_base__ = .;
      PROVIDE(HeapBegin = .);
      . = ORIGIN(CLR_MANAGED_HEAP_RAM) + LENGTH(CLR_MANAGED_HEAP_RAM);
      . = ALIGN(8);
      __clr_managed_heap_end__ = .;
      PROVIDE(HeapEnd = .);
  } > CLR_MANAGED_HEAP_RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

/* Code rules inclusion.*/
INCLUDE rules_code.ld

/* boot clipboard rules inclusion.*/
INCLUDE rules_bootclipboard.ld
