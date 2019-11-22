
#include "Display.h"
#include "DisplayInterface.h"
#include "GraphicsMemoryHeap.h"
#include "Graphics.h"

extern DisplayInterface g_DisplayInterface;
extern DisplayDriver g_DisplayDriver;
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
			g_DisplayInterface.InitializeDisplayInterface();
			g_DisplayDriver.Initialize();
		}
#ifdef __cplusplus
	}
}
#endif
