
#include "Display.h"
#include "DisplayInterface.h"
#include "TouchDevice.h"
#include "TouchInterface.h"
#include "GraphicsMemoryHeap.h"
#include "Graphics.h"

extern DisplayInterface g_DisplayInterface;
extern DisplayDriver g_DisplayDriver;

extern TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

extern GraphicsMemoryHeap g_GraphicsMemoryHeap;

#ifdef __cplusplus
extern "C" {
#endif
    extern "C" void GraphicsInitialize();

    extern "C"
    {
        void GraphicsInitialize()
        {
            g_GraphicsMemoryHeap.Initialize();

            g_DisplayInterface.Initialize();
            g_DisplayDriver.Initialize();

            //g_TouchInterface.Initialize();
            //g_TouchDevice.Initialize();
        }
#ifdef __cplusplus
    }
}
#endif
