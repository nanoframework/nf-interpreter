 SECTIONS
{
 
 /* nanoFramework CLR managed heap section at the specified RAM section.*/
    .clr_managed_heap (NOLOAD) :
    {
        . = ALIGN(8);
        PROVIDE(HeapBegin = LOADADDR(.clr_managed_heap));
        __clr_managed_heap_base__ = .;
        . += __clr_managed_heap_size__;
        . = ALIGN(8);
        __clr_managed_heap_end__ = .;
        PROVIDE(HeapEnd = .);
    } > CLR_MANAGED_HEAP_RAM

        /* RAM space reserved for the vector table */
    .RAMVectorTable (NOLOAD) : ALIGN(4)
    {
        *(.RAMVectorTable)
    } > ramvt
}