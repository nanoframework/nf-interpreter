 SECTIONS
{
 
    /* nanoFramework graphics managed heap section at the specified RAM section.*/
    .graphics_managed_heap (NOLOAD) :
    {
        . = ALIGN(8);
        __graphics_managed_heap_base__ = .;
        PROVIDE(GraphicsHeapBegin = LOADADDR(.graphics_managed_heap));
        . = ORIGIN(GRAPHICS_HEAP_RAM) + LENGTH(GRAPHICS_HEAP_RAM);
        . = ALIGN(8);
        __graphics_managed_heap_end__ = .;
        PROVIDE(GraphicsHeapEnd = .);
    } > GRAPHICS_HEAP_RAM


 .graphics_Video_frame_buffer (NOLOAD) :
    {
        . = ALIGN(8);
        __graphics_Video_frame_buffer_base__ = .;
        PROVIDE(GraphicsVideoFrameBufferBegin = LOADADDR(.graphics_Video_frame_buffer));
        . = ORIGIN(GRAPHICS_VIDEO_FRAME_BUFFER) + LENGTH(GRAPHICS_VIDEO_FRAME_BUFFER);
        . = ALIGN(8);
        __graphics_Video_frame_buffer_end__ = .;
        PROVIDE(GraphicsVideoFrameBufferEnd = .);
    } > GRAPHICS_VIDEO_FRAME_BUFFER

}
