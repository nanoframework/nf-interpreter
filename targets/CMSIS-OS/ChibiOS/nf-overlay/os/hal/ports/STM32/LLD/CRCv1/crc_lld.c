//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

///////////////////////////////////////////////////////////////////////////////
// This driver has been tested with the following STM32 series: F0
///////////////////////////////////////////////////////////////////////////////


// The reversed CRC32 calculation is an extra step required for F1, L1, F2 and F4 units series.
// Others have different configurations and polynomial coefficients.


#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_CRC == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////

#if (STM32_CRC_PROGRAMMABLE == TRUE) || \
    defined(STM32F7XX) || defined(STM32L0XX) || defined(STM32H7XX)

// CRC default configuration.
static const crcConfig defaultConfig = {

  .CRCLength                = CRC_POLYLENGTH_32B,
  .GeneratingPolynomial     = DEFAULT_CRC32_POLY,
  .InputDataFormat          = CRC_INPUTDATA_FORMAT_BYTES,
  .InputDataInversionMode   = CRC_INPUTDATA_INVERSION_NONE,
  .OutputDataInversionMode  = CRC_OUTPUTDATA_INVERSION_DISABLE
};
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////
CRCDriver CRCD1;

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

void crc_lld_init(void) {

    CRCD1.State  = CRC_STOP;
    CRCD1.Config = NULL;

    #if (CRC_USE_MUTUAL_EXCLUSION == TRUE)
    osalMutexObjectInit(&CRCD1.Lock);
    #endif    

    CRCD1.Instance = CRC;
}

void crc_lld_start(const crcConfig *config) {

    // enable peripheral
    rccEnableCRC(FALSE);

#if (STM32_CRC_PROGRAMMABLE == TRUE) || \
    defined(STM32F7XX) || defined(STM32L0XX) || defined(STM32H7XX)

    // set configuration, if supplied
    if (config == NULL)
    {
        CRCD1.Config = &defaultConfig;
    }
    else
    {
        CRCD1.Config = config;
    }

#else
  osalDbgAssert(config == NULL, "CRC peripheral doesn't support configuration");
#endif

#if (STM32_CRC_PROGRAMMABLE == TRUE) || \
    defined(STM32F7XX) || defined(STM32L0XX) || defined(STM32H7XX)

    WRITE_REG(CRCD1.Instance->INIT, DEFAULT_CRC_INITVALUE);

    // set generating polynomial
    WRITE_REG(CRCD1.Instance->POL, CRCD1.Config->GeneratingPolynomial);
    
    CRCD1.Instance->CR = 0;

    // set generating polynomial size
    MODIFY_REG(CRCD1.Instance->CR, CRC_CR_POLYSIZE, CRCD1.Config->CRCLength);  

    // set input data inversion mode
    MODIFY_REG(CRCD1.Instance->CR, CRC_CR_REV_IN, CRCD1.Config->OutputDataInversionMode); 

    // set output data inversion mode
    MODIFY_REG(CRCD1.Instance->CR, CRC_CR_REV_OUT, CRCD1.Config->OutputDataInversionMode);

#endif

    // driver is ready
    CRCD1.State = CRC_READY;
}

void crc_lld_stop() {

    // disable  peripheral
    rccDisableCRC();

    // driver is stopped
    CRCD1.State = CRC_STOP;
}

void crc_lld_reset() {

    CRCD1.Instance->CR |= CRC_CR_RESET;
}

uint32_t crc_lld_compute(const void* buffer, int size, uint32_t initialCrc) {

    int32_t index = 0U;
    uint32_t arg1;
    uint32_t crc = 0;

    // anything to do here?
    if(size == 0)
    {
        return initialCrc;
    }

    // get pointer to buffer
    uint8_t* ptr = (uint8_t*)buffer;

#if defined(STM32F1XX) || defined(STM32L1XX) || defined(STM32F2XX) || defined(STM32F4XX)
    uint32_t size_remainder = 0;

    // need to reset CRC peripheral if:
    // - CRC initial value is 0 
    // - the initial CRC is NOT already loaded in the calculation register 
    if(initialCrc == 0 || (CRCD1.Instance->DR != initialCrc))
    {
        // Reset CRC Calculation Unit
        crc_lld_reset();

        // CRC calculation unit is initiated with 0xFFFFFFFF which is not a initial value for our CRC calculation
        // feeding 0xFFFFFFFF to the calculation unit will set the register to 0x00000000
        while(CRCD1.Instance->DR != 0x0)
        {
             CRCD1.Instance->DR = CRCD1.Instance->DR;
        }
    }

    if(initialCrc != 0 && CRCD1.Instance->DR != initialCrc)
    {
        // we have an initial value for CRC calculation and that is not loaded in the CRC calculation register
        // load calculation register with REVERSE initial CRC32 value (because of STM32F4 shift order)
        CRCD1.Instance->DR = crc_lld_reverseCRC32(initialCrc);
    }

    // set variable to hold how many bytes remain after processing the buffer in steps of 4 bytes
    size_remainder = size & 3;

    // we'll be reading the buffer in steps of 4 bytes, so the size must be recalculated accordingly
    size = size >> 2;

    // feed data into the CRC calculator
    for(index = 0U; index < size; index++)
    {
        // take the next 4 bytes as if they were a UINT32
        // because the CRC calculation unit expects the bytes in reverse order, reverse the byte order first
        arg1 = __REV(*(uint32_t*)(ptr));

        // feed the bytes to the CRC
        CRCD1.Instance->DR = arg1;

        // copy to return value
        crc = (uint32_t)CRCD1.Instance->DR;

        // increase pointer by 4 to the next position
        // !! we are reading UINT32 from a UINT8 pointer!!
        ptr +=4;
    }

    // compute CRC for remaining bytes, if any
    while(size_remainder--)
    {
        crc = crc_lld_fastCRC32(crc, *(uint8_t*)(ptr++));
    }

#else
    uint16_t* ptr16 = (uint16_t*)buffer;
    
    // need to reset CRC peripheral if:
    // - CRC initial value is 0 
    // - the initial CRC is NOT already loaded in the calculation register 
    if(initialCrc != DEFAULT_CRC_INITVALUE || (CRCD1.Instance->DR != initialCrc))
    {
        // Reset CRC Calculation Unit
        crc_lld_reset();

  #if (STM32_CRC_PROGRAMMABLE == TRUE) || \
    defined(STM32F7XX) || defined(STM32L0XX) || defined(STM32H7XX)
        // set initial CRC value
        WRITE_REG(CRCD1.Instance->INIT, initialCrc);
  #endif 
    }
    
    switch (CRCD1.Config->InputDataFormat)
    {
        case CRC_INPUTDATA_FORMAT_WORDS:  
            // Enter 32-bit input data to the CRC calculator
            for(index = 0; index < size; index++)
            {
                arg1 = ((uint32_t*)buffer)[index];
                CRCD1.Instance->DR = arg1;
            }
            break;
        
        case CRC_INPUTDATA_FORMAT_BYTES: 
            // Specific 8-bit input data handling
            
            /* Processing time optimization: 4 bytes are entered in a row with a single word write,
            * last bytes must be carefully fed to the CRC calculator to ensure a correct type
            * handling by the IP */
            for(index = 0; index < (size / 4); index++)
            {
                CRCD1.Instance->DR = (uint32_t)(((uint32_t)(ptr[4*index])<<24) | ((uint32_t)(ptr[4*index + 1])<<16) | ((uint32_t)(ptr[4*index + 2])<<8) | (uint32_t)(ptr[4*index + 3]));
            }

            // last bytes specific handling
            if((size % 4) != 0)
            {
                if(size % 4 == 1)
                {
                    CRCD1.Instance->DR = ptr[4*index];
                }
                if(size % 4 == 2)
                {
                    CRCD1.Instance->DR = (uint16_t)((uint16_t)((uint16_t)(ptr[4*index])<<8) | (uint16_t)(ptr[4*index + 1]));
                }
                if(size % 4 == 3)
                {
                    CRCD1.Instance->DR = (uint16_t)((uint16_t)((uint16_t)(ptr[4*index])<<8) | (uint16_t)(ptr[4*index + 1]));
                    CRCD1.Instance->DR = ptr[4*index + 2];       
                }
            }
            break;
        
        case CRC_INPUTDATA_FORMAT_HALFWORDS: 
            // Specific 16-bit input data handling
            
            /* Processing time optimization: 2 HalfWords are entered in a row with a single word write,
            * in case of odd length, last HalfWord must be carefully fed to the CRC calculator to ensure 
            * a correct type handling by the IP */
            for(index = 0; index < (size / 2); index++)
            {
                CRCD1.Instance->DR = (((uint32_t)(ptr16[2*index])<<16) | (uint32_t)(ptr16[2*index + 1]));
            }
            if((size % 2) != 0)
            {
                CRCD1.Instance->DR = ptr16[2*index]; 
            }
            break;
    }

    crc = CRCD1.Instance->DR;

#endif    

    // Return the CRC computed value
    return crc;
}

#if (CRC_USE_MUTUAL_EXCLUSION == TRUE)

void crc_lld_aquire() {
    osalMutexLock(&CRCD1.Lock);
}

void crc_lld_release() {
    osalMutexUnlock(&CRCD1.Lock);
}

#endif

uint32_t crc_lld_reverseCRC32(uint32_t targetCRC)
{
    // nibble lookup table for _REVERSE_ CRC32 polynomial 0x4C11DB7
    static const uint32_t crc32NibbleTable[16] =
    {
        0x00000000, 0xB2B4BCB6, 0x61A864DB, 0xD31CD86D, 0xC350C9B6, 0x71E47500, 0xA2F8AD6D, 0x104C11DB,
        0x82608EDB, 0x30D4326D, 0xE3C8EA00, 0x517C56B6, 0x4130476D, 0xF384FBDB, 0x209823B6, 0x922C9F00
    };
    
    uint8_t counter = 8;

    while(counter--)
    {
        targetCRC = (targetCRC >> 4) ^ crc32NibbleTable[targetCRC & 0x0F];
    }

    return targetCRC;
}

uint32_t crc_lld_fastCRC32(uint32_t initialCrc, uint8_t data)
{
    // nibble lookup table for CRC32 polynomial 0x4C11DB7
    static const uint32_t crc32NibbleTable[16] = 
    { 
        0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
        0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD 
    };

    initialCrc = crc32NibbleTable[(initialCrc >> 28) ^ (data >> 4)] ^ (initialCrc << 4) ;
    initialCrc = crc32NibbleTable[(initialCrc >> 28) ^ (data & 0xF)] ^ (initialCrc << 4) ;

    return initialCrc;
}

#endif
