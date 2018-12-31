////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVERS_GPIO_DECL_H_
#define _DRIVERS_GPIO_DECL_H_ 1


#include "nanoCLR_Types.h"

#define GPIO_PIN_NONE               0xFFFFFFFF

#define GPIO_ATTRIBUTE_NONE         0x00
#define GPIO_ATTRIBUTE_INPUT        0x01
#define GPIO_ATTRIBUTE_OUTPUT       0x02
#define GPIO_ATTRIBUTE_ALTERNATE_A  0x04
#define GPIO_ATTRIBUTE_ALTERNATE_B  0x08

enum GPIO_RESISTOR
{
    RESISTOR_DISABLED = 0,
    RESISTOR_PULLDOWN = 1,
    RESISTOR_PULLUP = 2
};

enum GPIO_INT_EDGE
{
    GPIO_INT_NONE       = 0,
    GPIO_INT_EDGE_LOW   = 1,
    GPIO_INT_EDGE_HIGH  = 2,
    GPIO_INT_EDGE_BOTH  = 3,
    GPIO_INT_LEVEL_HIGH = 4,
    GPIO_INT_LEVEL_LOW  = 5
};

struct GPIO_FLAG_RESISTOR
{
    GPIO_PIN      Pin;
    bool          ActiveState;
    GPIO_RESISTOR Resistor;
};

// Generic enum for GPIO Primary/Alternate/Mux function 
// Different processor have different defintion for the mode
enum GPIO_ALT_MODE
{
    GPIO_ALT_PRIMARY    = 0,
    GPIO_ALT_MODE_1     = 1,
    GPIO_ALT_MODE_2     = 2,
    GPIO_ALT_MODE_3     = 3,
    GPIO_ALT_MODE_4     = 4,
    GPIO_ALT_MODE_5     = 5,
    GPIO_ALT_MODE_6     = 6,
    GPIO_ALT_MODE_7     = 7,
    GPIO_ALT_MODE_8     = 8,
};

typedef void (*GPIO_INTERRUPT_SERVICE_ROUTINE)( GPIO_PIN Pin, bool PinState, void* Param );

bool   CPU_GPIO_Initialize     ();
bool   CPU_GPIO_Uninitialize   ();
CLR_UINT32 CPU_GPIO_Attributes     ( GPIO_PIN Pin );
void   CPU_GPIO_DisablePin     ( GPIO_PIN Pin, GPIO_RESISTOR ResistorState, CLR_UINT32 Direction, GPIO_ALT_MODE AltFunction );
void   CPU_GPIO_EnableOutputPin( GPIO_PIN Pin, bool InitialState );
bool   CPU_GPIO_EnableInputPin ( GPIO_PIN Pin, bool GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE PIN_ISR, GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState );
bool   CPU_GPIO_EnableInputPin2( GPIO_PIN Pin, bool GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE PIN_ISR, void* ISR_Param, GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState );
bool   CPU_GPIO_GetPinState    ( GPIO_PIN Pin );
void   CPU_GPIO_SetPinState    ( GPIO_PIN Pin, bool PinState );
bool   CPU_GPIO_PinIsBusy      ( GPIO_PIN Pin );
bool   CPU_GPIO_ReservePin     ( GPIO_PIN Pin, bool fReserve );
CLR_UINT32 CPU_GPIO_GetDebounce    ();
bool   CPU_GPIO_SetDebounce    ( CLR_INT64 debounceTimeMilliseconds );
CLR_INT32  CPU_GPIO_GetPinCount    ();
void   CPU_GPIO_GetPinsMap     ( CLR_UINT8* pins, size_t size );
CLR_UINT8  CPU_GPIO_GetSupportedResistorModes(GPIO_PIN pin );
CLR_UINT8  CPU_GPIO_GetSupportedInterruptModes(GPIO_PIN pin );

CLR_UINT32 CPU_GPIO_GetPinDebounce( GPIO_PIN Pin );
bool   CPU_GPIO_SetPinDebounce( GPIO_PIN Pin, CLR_INT64 debounceTimeMilliseconds );

#endif // _DRIVERS_GPIO_DECL_H_

