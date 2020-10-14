#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
 
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.Events")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/include")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Displays")
list(APPEND nanoFramework.Graphics_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Devices")

 set (  nanoFramework.Graphics_SRCS
        nanoPAL_Events_functions.cpp
        nanoPAL_Events_driver.cpp
        Bitmap_Decoder.cpp
        Font.cpp
        Gif.cpp
        GifDecoder.cpp
        lzwread.cpp
        jbytearraydatasrc.c
        jcapimin.c
        jcapistd.c
        jccoefct.c
        jccolor.c
        jcdctmgr.c
        jchuff.c
        jcinit.c
        jcmainct.c
        jcmarker.c
        jcmaster.c
        jcomapi.c
        jcparam.c
        jcphuff.c
        jcprepct.c
        jcsample.c
        jctrans.c
        jdapimin.c
        jdapistd.c
        jdcoefct.c
        jdcolor.c
        jddctmgr.c
        jdhuff.c
        jdinput.c
        jdmainct.c
        jdmarker.c
        jdmaster.c
        jdmerge.c
        jdphuff.c
        jdpostct.c
        jdsample.c
        jdtrans.c
        jerror.c
        jfdctflt.c
        jfdctfst.c
        jfdctint.c
        jidctflt.c
        jidctfst.c
        jidctint.c
        jidctred.c
        jmemmgr.c
        jmemnanoclr.cpp
        Jpeg.cpp
        jquant1.c
        jquant2.c
        jutils.c
        mcbcr.c
        mfint.c
        miint.c
        pfint.c
        piint.c
        transupp.c
        Graphics.cpp
        GraphicsDriver.cpp
        GraphicsMemoryHeap.cpp
        
        nanoFramework_Graphics.cpp
        nanoFramework_Graphics_nanoFramework_UI_Ink.cpp
        nanoFramework_Graphics_nanoFramework_UI_TouchCollectorConfiguration.cpp
        nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor.cpp
        nanoFramework_Graphics_nanoFramework_UI_TouchPanel.cpp
        nanoFramework_Graphics_nanoFramework_UI_Bitmap.cpp
        nanoFramework_Graphics_nanoFramework_UI_DisplayControl.cpp
        nanoFramework_Graphics_nanoFramework_UI_Font.cpp
        
        TouchPanel.cpp
        Gestures.cpp
        Ink.cpp
        
        #Common Display/Touch Code
        "${GRAPHICS_DISPLAY}"
        "${TOUCHPANEL_DEVICE}"
        
        #Target board Display/Touch Code
        Graphics_Memory.cpp
        "${GRAPHICS_DISPLAY_INTERFACE}"
        "${TOUCHPANEL_INTERFACE}"
)

foreach(SRC_FILE ${nanoFramework.Graphics_SRCS})
    set(nanoFramework.Graphics_SRC_FILE ${SRC_FILE}-NOTFOUND)
    find_file(nanoFramework.Graphics_SRC_FILE ${SRC_FILE}
        PATHS 
        "${PROJECT_SOURCE_DIR}/src/PAL/Events"
        "${PROJECT_SOURCE_DIR}/src/CLR/Core"
        
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Bmp"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Fonts"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Displays"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core"
        "${PROJECT_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Devices"

        "${TARGET_BASE_LOCATION}/nanoCLR/nanoFramework.Graphics" 

        CMAKE_FIND_ROOT_PATH_BOTH     )

        if( ${nanoFramework.Graphics_SRC_FILE} STREQUAL "nanoFramework.Graphics_SRC_FILE-NOTFOUND")
           message( "___________________________________________________________________________")
           message( "Cannot find file : ${SRC_FILE} in FindnanoFramework.Graphics.cmake")
           message( "___________________________________________________________________________")
        endif()

    list(APPEND nanoFramework.Graphics_SOURCES ${nanoFramework.Graphics_SRC_FILE} )
endforeach()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Graphics DEFAULT_MSG nanoFramework.Graphics_INCLUDE_DIRS nanoFramework.Graphics_SOURCES)
