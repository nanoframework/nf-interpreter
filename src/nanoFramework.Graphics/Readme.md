
| **Graphics and LCD**  | Can handle graphics natively or from the managed application. Managed applications use the Windows Presentation Foundation model for dynamic layout and event dispatching. Native-level bitmaps are based on and include the basic primitives for drawing ellipsis and rectangles in a customizable source code format. Supports using bitmaps as resources in the nanoCLRBitmap (an internal format), BMP, JPG and GIF formats. The following bitmap types are supported. |
|-----------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|


-   24bpp RGB,

-   32bpp RGB,

-   48bpp RGB,

-   16bpp RGB (565 format)

-   15bpp RGB (555) format;

-   1bpp monochrome

All color bitmap formats are silently converted to 16bpp RGB (565) at build time
for the managed application. 1bpp monochrome bitmaps are compressed at
build-time.

When loading bitmap data at run-time:

-   Only 16bpp (565 format) and 24bpp RGB formats are supported by
    BitmapImageType.Bmp.

-   The only supported mechanism for loading 1bpp monochrome bitmaps is
    embedding them as bitmap resources at compile time.

-   Bitmap size is unlimited.

>   **Touch Screen** Standardized interface for hardware that uses a touch
>   screen and stylus, including gestures and multi-touch support.

| **Graphical User Interfaces (GUIs)**  | Offers a subset of the Windows Presentation Foundation (WFP) classes.                                                                                            |
|---------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------|

