////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVERS_SPI_DECL_H_
#define _DRIVERS_SPI_DECL_H_ 1

#include "nanoCLR_Types.h"


#define SPI_CTRL_DEFAULT 0

struct SPI_CONFIGURATION
{
    GPIO_PIN       DeviceCS;
    bool           CS_Active;             // False = LOW active,      TRUE = HIGH active
    bool           MSK_IDLE;              // False = LOW during idle, TRUE = HIGH during idle
    bool           MSK_SampleEdge;        // False = sample falling edge,  TRUE = samples on rising
    bool           MD_16bits;
    CLR_UINT32         Clock_RateKHz;
    CLR_UINT32         CS_Setup_uSecs;
    CLR_UINT32         CS_Hold_uSecs;
    CLR_UINT32         SPI_mod;
    GPIO_FLAG      BusyPin;
};

struct SPI_XACTION_16
{
    CLR_UINT16* Write16;
    CLR_INT32   WriteCount;
    CLR_UINT16* Read16;
    CLR_INT32   ReadCount;
    CLR_INT32   ReadStartOffset;
    CLR_UINT32  SPI_mod;
    GPIO_FLAG BusyPin;
};

struct SPI_XACTION_8
{
    CLR_UINT8* Write8;
    CLR_INT32  WriteCount;
    CLR_UINT8* Read8;
    CLR_INT32  ReadCount;
    CLR_INT32  ReadStartOffset;
    CLR_UINT32 SPI_mod;
    GPIO_FLAG BusyPin;
};

//bool   CPU_SPI_Initialize      ();
//void   CPU_SPI_Uninitialize    ();
//bool   CPU_SPI_nWrite16_nRead16( const SPI_CONFIGURATION& Configuration, CLR_UINT16* Write16, CLR_INT32 WriteCount, CLR_UINT16* Read16, CLR_INT32 ReadCount, CLR_INT32 ReadStartOffset );
//bool   CPU_SPI_nWrite8_nRead8  ( const SPI_CONFIGURATION& Configuration, UINT8* Write8, CLR_INT32 WriteCount, UINT8* Read8, CLR_INT32 ReadCount, CLR_INT32 ReadStartOffset );
//bool   CPU_SPI_Xaction_Start   ( const SPI_CONFIGURATION& Configuration );
//bool   CPU_SPI_Xaction_Stop    ( const SPI_CONFIGURATION& Configuration );
//bool   CPU_SPI_Xaction_nWrite16_nRead16( SPI_XACTION_16& Transaction );
//bool   CPU_SPI_Xaction_nWrite8_nRead8  ( SPI_XACTION_8& Transaction  );
//CLR_UINT32 CPU_SPI_PortsCount      ();
//void   CPU_SPI_GetPins         ( CLR_UINT32 spi_mod, GPIO_PIN& msk, GPIO_PIN& miso, GPIO_PIN& mosi );
//
//CLR_UINT32 CPU_SPI_MinClockFrequency( CLR_UINT32 spi_mod );
//CLR_UINT32 CPU_SPI_MaxClockFrequency( CLR_UINT32 spi_mod );
//CLR_UINT32 CPU_SPI_ChipSelectLineCount( CLR_UINT32 spi_mod );

#endif // _DRIVERS_SPI_DECL_H_

