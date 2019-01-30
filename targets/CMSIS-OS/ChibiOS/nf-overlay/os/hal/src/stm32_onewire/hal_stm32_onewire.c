//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/*
 * Hardware Abstraction Layer for ONEWIRE Unit
 */
#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_ONEWIRE == TRUE)

#include <string.h>

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define PAGE_LENGTH 64
#define PAGE_LENGTH_HYGRO 32

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

uint32_t utilcrc16 = 0;
uint8_t utilcrc8;

// local variables for this module to hold search state information

uint8_t LastDiscrepancy;
uint8_t LastFamilyDiscrepancy;
uint8_t LastDevice;

uint8_t SerialNum[8];

#if ONEWIRE_CRC8_TABLE

static const uint8_t dscrc_table[] = {
  0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
  157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
  35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
  190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
  70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
  219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
  101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
  248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
  140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
  17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
  175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
  50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
  202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
  87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
  233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
  116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

#endif // ONEWIRE_CRC8_TABLE

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/


// Initializes the ONEWIRE Driver
void oneWireInit(void) {
  oneWire_lld_init();
}

// Configures and activates the CRC peripheral
void oneWireStart() {

    // better lock this to setup the driver and start the peripheral
    osalSysLock();
    
    oneWire_lld_start();

    osalSysUnlock();
}

// Deactivates the ONEWIRE peripheral
void oneWireStop() {

    // better lock this to stop the peripheral
    osalSysLock();
  
    oneWire_lld_stop();
    
    osalSysUnlock();
}


#if (ONEWIRE_USE_MUTUAL_EXCLUSION == TRUE)

void oneWireAcquireModule() {

  oneWire_lld_aquire();
}

void oneWireReleaseModule() {

  oneWire_lld_release();  
}

#endif /* ONEWIRE_USE_MUTUAL_EXCLUSION == TRUE */

// Calculate a new CRC16 from the input data short. Return the current
// CRC16 and also update the global variable CRC16.
uint32_t docrc16(uint32_t cdata)
{
    uint8_t oddparity[] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

    cdata = (cdata ^ (utilcrc16 & 0xff)) & 0xff;
    utilcrc16 >>= 8;

    if ((oddparity[(cdata & 0xf)] ^ oddparity[(cdata >> 4)]) == 1)
    {
        utilcrc16 ^= 0xC001;
    }

    cdata <<= 6;
    utilcrc16 ^= cdata;
    cdata <<= 1;
    utilcrc16 ^= cdata;

    return utilcrc16;
}

// Reset CRC16 to the value passed in
void setcrc16(uint32_t reset)
{
    utilcrc16 = reset;
}

#if ONEWIRE_CRC8_TABLE

// compute CRC8 using lookup table (FLASH expensive)

uint8_t doCrc8(uint8_t oldCrc, uint8_t x)
{
    return dscrc_table[oldCrc ^ x];
}

#else

// compute CRC8 using running algorith (slower but saves FLASH)

uint8_t doCrc8(uint8_t oldCrc, uint8_t x)
{
    uint8_t crc = oldCrc;

    for (uint8_t i = 8; i; i--)
    {
        uint8_t mix = (crc ^ x) & 0x01;
        crc >>= 1;
        if (mix) crc ^= 0x8C;
        x >>= 1;
    }
    
    return crc;
}

#endif // ONEWIRE_CRC8_TABLE


/*******************************************************************************
** NAME: oneWireTouchReset *************************************************
********************************************************************************

DESCRIPTION:
Reset all of the devices on the 1-Wire Net and return the result.

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:
--------------------------------------------------------------------------
 
 Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
          FALSE(0): no presense pulses detected

*******************************************************************************/
uint8_t oneWireTouchReset(void) 
{
    return oneWire_lld_TouchReset();
}

/*******************************************************************************
** NAME: oneWireTouchBit ***************************************************
********************************************************************************

DESCRIPTION:
 * Send 1 bit of communication to the 1-Wire Net and return the
 * result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
 * least significant bit is used and the least significant bit
 * of the result is the return bit.
 * @return a SMALLINT with  0 bit read from sendbit or 1 bit read fromsendbit
 * @param sendbit the least significant bit is the bit to send

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
bool oneWireTouchBit(bool sendbit) 
{
    return oneWire_lld_TouchBit(sendbit);
}

/*******************************************************************************
** NAME: oneWireTouchByte **************************************************
********************************************************************************

DESCRIPTION:
  // Send 8 bits of communication to the 1-Wire Net and return the
  // result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
  // least significant 8 bits are used and the least significant 8 bits
  // of the result is the return byte.
  //
  // 'sendbyte'   - 8 bits to send (least significant byte)
  //
  // Returns:  8 bits read from sendbyte

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireTouchByte(uint8_t sendbyte)
{
    return oneWire_lld_TouchByte(sendbyte);
}

/*******************************************************************************
** NAME: oneWireWriteByte **************************************************
********************************************************************************

DESCRIPTION:
  // Send 8 bits of communication to the 1-Wire Net and verify that the
  // 8 bits read from the 1-Wire Net is the same (write operation).
  // The parameter 'sendbyte' least significant 8 bits are used.
  //
  // 'sendbyte'   - 8 bits to send (least significant byte)
  //
  // Returns:  TRUE: bytes written and echo was the same
  //           FALSE: echo was not the same

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireWriteByte(uint8_t sendbyte)
{
  return (oneWireTouchByte(sendbyte) == sendbyte) ? TRUE : FALSE;
}

/*******************************************************************************
** NAME: oneWireReadByte ***************************************************
********************************************************************************

DESCRIPTION:
  // Send 8 bits of read communication to the 1-Wire Net and and return the
  // result 8 bits read from the 1-Wire Net.
  //
  // Returns:  8 bytes read from 1-Wire Net

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireReadByte(void)
{
  return oneWireTouchByte(0xFF);
}

/*******************************************************************************
** NAME: oneWireSerialNum **************************************************
********************************************************************************

DESCRIPTION:
     // The 'oneWireSerialNum' function either reads or sets the SerialNum buffer
     // that is used in the search functions 'owFirst' and 'owNext'.
     // This function contains two parameters, 'serialnum_buf' is a pointer
     // to a buffer provided by the caller.  'serialnum_buf' should point to
     // an array of 8 unsigned chars.  The second parameter is a flag called
     // 'do_read' that is TRUE (1) if the operation is to read and FALSE
     // (0) if the operation is to set the internal SerialNum buffer from
     // the data in the provided buffer.
     //
     // 'serialnum_buf' - buffer to that contains the serial number to set
     //                   when do_read = FALSE (0) and buffer to get the serial
     //                   number when do_read = TRUE (1).
     // 'do_read'       - flag to indicate reading (1) or setting (0) the current
     //                   serial number.
     //

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
void oneWireSerialNum(uint8_t* serialnum_buf, uint8_t do_read)
{
     uint8_t i;
     
     //-------------------------------------------------//
     // read the internal buffer and place in 'serialnum_buf'
     if (do_read) 
     {
         for (i = 0; i < 8; i++)
         {
             serialnum_buf[i] = SerialNum[i];
         }
     }
     // set the internal buffer from the data in 'serialnum_buf'
     else 
     {
         for (i = 0; i < 8; i++)
         {
             SerialNum[i] = serialnum_buf[i];
         }
     }
}

/*******************************************************************************
** NAME: oneWireAccess *****************************************************
********************************************************************************

DESCRIPTION:
  //--------------------------------------------------------------------------
  // The 'owAccess' function resets the 1-Wire and sends a MATCH Serial
  // Number command followed by the current SerialNum code. After this
  // function is complete the 1-Wire device is ready to accept device-specific
  // commands.
  //
  // Returns:   TRUE (1) : reset indicates present and device is ready
  //                       for commands.
  //            FALSE (0): reset does not indicate presence or echos 'writes'
  //                       are not correct.
  //

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireAccess(void)
{
  uint8_t sendpacket[9];
  uint8_t i;
  
  //
  //-------------------------------------------------//
  // reset the 1-wire
  if (oneWireTouchReset()) 
  {
      // create a buffer to use with block function
      // match Serial Number command 0x55
      sendpacket[0] = MATCH_ROM;
      // Serial Number
      for (i = 1; i < 9; i++)
      {
          sendpacket[i] = SerialNum[i-1];
      }
      
      // send/recieve the transfer buffer
      if (oneWireBlock(FALSE, sendpacket, 9)) 
      {
          // verify that the echo of the writes was correct
          for (i = 1; i < 9; i++)
          {
            if (sendpacket[i] != SerialNum[i-1])
            {
              return FALSE;
            }
          }
        
          if (sendpacket[0] != MATCH_ROM) 
          {
              //OWERROR(//OWERROR_WRITE_VERIFY_FAILED);
              return FALSE;
          }
          else
          {
              return TRUE;
          }
      }
  }
    
  // reset or match echo failed
  return FALSE;
}

/*******************************************************************************
** NAME: oneWireBlock ******************************************************
********************************************************************************

DESCRIPTION:
  // The 'oneWireBlock' transfers a block of data to and from the
  // 1-Wire Net with an optional reset at the begining of communication.
  // The result is returned in the same buffer.
  //
  // 'doReset' - cause a oneWireTouchReset to occure at the begining of
  //              communication TRUE(1) or not FALSE(0)
  // 'tran_buf' - pointer to a block of unsigned
  //              chars of length 'TranferLength' that will be sent
  //              to the 1-Wire Net
  // 'tran_len' - length in bytes to transfer
  // Supported devices: all
  //
  // Returns:   TRUE (1) : The optional reset returned a valid
  //                       presence (doReset == TRUE) or there
  //                       was no reset required.
  //            FALSE (0): The reset did not return a valid prsence
  //                       (doReset == TRUE).
  //
  //  The maximum tran_len is 160

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireBlock(uint8_t doReset, uint8_t* tran_buf, uint8_t tran_len)
{
  uint8_t i;
  //
  //-------------------------------------------------//
  // check for a block too big
  if (tran_len > 160) 
  {
      // error: BLOCK TOO BIG
      return FALSE;
  }
  
  // check if need to do a owTouchReset first
  if (doReset)
  {
      if (!oneWireTouchReset())
      {
          // error: NO DEVICES ON NET
          return FALSE;
      }
  }
  
  // send and receive the buffer
  for (i = 0; i < tran_len; i++)
  {
      tran_buf[i] = oneWireTouchByte(tran_buf[i]);
  }
  
  return TRUE;
}

/*******************************************************************************
** NAME: oneWireWriteMemory **************************************************
********************************************************************************

DESCRIPTION:
write block of memory to iButton memory

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireWriteMemory(uint8_t* buf, uint32_t ln, uint32_t adr) 
{
  //-------------------------------------------------//
  // write to scratch and then copy
  if (oneWireWriteScratchpad(buf, ln, adr)) 
      return oneWireCopyScratchpad(ln, adr);
  
  // end function
  return FALSE;
}

/*******************************************************************************
** NAME: oneWireCopyScratchpad ***********************************************
********************************************************************************

DESCRIPTION:
copy scratchpad contempts

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:
Only valid for DS1921G,H,Z

*******************************************************************************/
uint8_t oneWireCopyScratchpad(uint32_t ln, uint32_t adr) 
{
  // local variables
  uint8_t i;
  uint8_t pbuf[15];

  //-------------------------------------------------//
  // check for presence indicator 
  if (oneWireAccess()) 
  {
      // construct a packet to send 
      pbuf[0] = COPY_SCRPAD;           // copy scratch command 
      pbuf[1] = (adr & 0xFF);          // address 1 
      pbuf[2] = ((adr >> 8) & 0xFF);   // address 2 
      pbuf[3] = (adr + ln - 1) & 0x1F; // offset 
      for (i = 0; i <= 9; i++)
          pbuf[4 + i] = 0xFF;           // result of copy 
      // perform the block 
      if (oneWireBlock(FALSE, pbuf, 14)) {
          if ((pbuf[13] == 0x55) || (pbuf[13] == 0xAA)) 
              return TRUE;
      }
  }
  
  // end function
  return FALSE;
}

/*******************************************************************************
** NAME: oneWireWriteScratchpad **********************************************
********************************************************************************

DESCRIPTION:
copy scratchpad contempts

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:
Only valid for DS1921G,H,Z

*******************************************************************************/
uint8_t oneWireWriteScratchpad(uint8_t* buf, uint32_t ln, uint32_t adr)
{
  uint8_t i;
  uint8_t pbuf[40];
  
  //-------------------------------------------------//
  // check for presence indicator 
  if (oneWireAccess())
  {
    // construct a packet to send  
    pbuf[0] = WR_SCRPAD; // write scratch command 
    pbuf[1] = (adr & 0xFF); // address 1 
    pbuf[2] = ((adr >> 8) & 0xFF); // address 2 
    
    // the write bytes 
    for (i = 0; i < ln; i++)
        pbuf[3 + i] = (uint8_t)(buf[i]); // data 
    
    // perform the block 
    if (!oneWireBlock(FALSE, pbuf, ln+3))
        return FALSE;
    
    // Now read back the scratch 
    if (oneWireAccess()) {
        // construct a packet to send 
        pbuf[0] = RD_SCRPAD; // read scratch command 
        pbuf[1] = 0xFF; // address 1 
        pbuf[2] = 0xFF; // address 2 
        pbuf[3] = 0xFF; // offset 
        
        // the write bytes 
        for (i = 0; i < ln; i++)
            pbuf[4 + i] = 0xFF; // data 
        
        // perform the block  
        if (!oneWireBlock(FALSE, pbuf, ln+4))
        return FALSE;
        
        // read address 1 
        if (pbuf[1] != (adr & 0xFF)) 
            return FALSE;
        // read address 2 
        if (pbuf[2] != ((adr >> 8) & 0xFF)) 
            return FALSE;
        // read the offset 
        if (pbuf[3] != ((adr + ln - 1) & 0x1F)) 
            return FALSE;
        // read and compare the contents 
        for (i = 0; i < ln; i++) {
            if (pbuf[4 + i] != buf[i]) 
                return FALSE;
        }
        // success
        return TRUE;
    }
  }

  // end function
  return FALSE;
        
}

/*******************************************************************************
** NAME: oneWireReadPage *****************************************************
********************************************************************************

DESCRIPTION
read memory page

USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:
Only valid for DS1921G,H,Z
Simplification of addressing using SKIP ROM

*******************************************************************************/
uint8_t oneWireReadPage(uint32_t start_pg, uint8_t* finalbuf) 
{
  uint32_t lastcrc16;
  uint32_t len,i;
  
  // create a packet to read a page
  len = 0;
  setcrc16(0x0);
  // skip ROM
  finalbuf[len++] = SKIP_ROM; 
  // read memory with crc command 
  finalbuf[len] = RD_MEMORY_CRC; 
  docrc16(finalbuf[len++]);         
  
  // address
  finalbuf[len] = (uint8_t)((start_pg << 5) & 0xFF);
  docrc16(finalbuf[len++]);         
  finalbuf[len] = (uint8_t)(start_pg >> 3); 
  docrc16(finalbuf[len++]); 
  
  // set 32 reads for data and 2 for crc
  for (i = 0; i < 34; i++)
  {
    finalbuf[len++] = 0xFF; 
  }
  // send the bytes
  if (oneWireBlock(TRUE , finalbuf, len)) 
  {
    // calculate the CRC over the last 34 bytes
    for (i = 0; i < 34; i++)
    {
      lastcrc16 = docrc16(finalbuf[len - 34 + i]);
    }
    // check crc
    if (lastcrc16 == 0xB001) 
    {
      // copy the data into the buffer without the command and parameters
      for (i = 0; i < 32; i++)
      {
        finalbuf[i] = finalbuf[i + 4];
      }
    }
    else
    {
      return FALSE;
    }
  }
  else
  {   
    return FALSE;
  }
  
  // end function
  return TRUE;
}

/*******************************************************************************
** NAME: oneWireReadPageCRCEE77 **********************************************
********************************************************************************

DESCRIPTION
 Read a complete memory page with CRC verification provided by the
 device.  Not supported by all devices.  See the method
 'hasPageAutoCRCEE()'.

 bank     to tell what memory bank of the ibutton to use.
 portnum  the port number of the port being used for the
          1-Wire Network.
 SNum     the serial number for the part.
 psw      8 byte password
 page     the page to read
 rd_cont  if 'true' then device read is continued without
          re-selecting.  This can only be used if the new
          read() continious where the last one led off
          and it is inside a 'beginExclusive/endExclusive'
          block.
 buff     byte array containing data that was read

 @return - returns '0' if the read page wasn't completed.
                   '1' if the operation is complete.


USAGE EXAMPLES: 

AUTHOR: jassimoes

COMMENTS:

*******************************************************************************/
uint8_t oneWireReadPageCRCEE77(uint32_t page, uint8_t *buff)
{
  uint8_t i, send_len=0, lsCRC16;
  uint8_t raw_buf[15];
  uint32_t str_add;
  uint32_t lastcrc16;
  
  // select the device
  if (!oneWireAccess())
  {
     return FALSE;
  }
  
  // command, address, offset, password (except last byte)
  raw_buf[send_len++] = READ_MEMORY_PSW_COMMAND;
  if(SerialNum[0] == 0x37)
  {
      str_add = page * PAGE_LENGTH;
  }
  else
  {
      str_add = page * PAGE_LENGTH_HYGRO;
  }
  raw_buf[send_len++] = str_add & 0xFF;
  raw_buf[send_len++] = ((str_add & 0xFFFF) >> 8) & 0xFF;

  // calculate the CRC16
  setcrc16(0);
  for(i = 0; i < send_len; i++)
      lastcrc16 = docrc16(raw_buf[i]);

  for (i = 0; i < 8; i++)
      raw_buf[send_len++] = 0xFF;


  if(SerialNum[0] == 0x37)
  {
      // send block (check copy indication complete)
      if(!oneWireBlock(FALSE,(uint8_t*)raw_buf,(send_len-1)))
      {
        return FALSE;
      }

  }
  else
  {
      if(!oneWireBlock(FALSE,(uint8_t*)raw_buf,send_len))
      {
        return FALSE;
      }
  }

  // set the read bytes
  if(SerialNum[0] == 0x37)
  {

    for (i = 0; i < PAGE_LENGTH; i++)
        buff[i] = 0xFF;
  }
  else
  {
    for(i=0; i<PAGE_LENGTH_HYGRO; i++)
        buff[i] = 0xFF;
  }

  // read the data
  if(SerialNum[0] == 0x37)
  {
    if(!oneWireBlock(FALSE,(uint8_t*)buff,PAGE_LENGTH))
    {
        return FALSE;
    }
  }
  else
  {
    if(!oneWireBlock(FALSE,(uint8_t*)buff,PAGE_LENGTH_HYGRO))
    {
        return FALSE;
    }
  }
  // read the first CRC16 byte
  lsCRC16 = oneWireReadByte();

  // calculate CRC on data read
  if(SerialNum[0] == 0x37)
  {
    for(i = 0; i < PAGE_LENGTH; i++)
        lastcrc16 = docrc16(buff[i]);
  }
  else
  {
    for(i = 0; i < PAGE_LENGTH_HYGRO; i++)
        lastcrc16 = docrc16(buff[i]);
  }

  // check lsByte of the CRC
  if ((lastcrc16 & 0xFF) != (~lsCRC16 & 0xFF))
  {
    return FALSE;
  }

  return TRUE;
}

/*******************************************************************************
** NAME: oneWireFindFirst ******************************************************
********************************************************************************/
// The 'oneWireFindFirst' finds the first device on the 1-Wire Net.
// This function contains one parameter 'alarmOnly'.  When 
// 'alarmOnly' is TRUE (1) the find alarm command 0xEC is 
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state. 
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'doReset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search. 
// 'alarmOnly' - TRUE (1) the find alarm command 0xEC is 
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's 
//                       Serial Number placed in the global SerialNum
//            FALSE (0): There are no devices on the 1-Wire Net.
bool oneWireFindFirst (bool doReset, bool alarmOnly)
{
    // reset the search state
    LastDiscrepancy = 0;
    LastDevice = FALSE;
    LastFamilyDiscrepancy = 0; 

    // Call Next and return it's return value;
    return oneWireFindNext(doReset, alarmOnly);
}

/*******************************************************************************
** NAME: oneWireFindNext *******************************************************
********************************************************************************/
// The 'oneWireFindNext' function does a general search.
// This function continues from the previos search state. The search state
// can be reset by using the 'oneWireFindFirst' function.
// This function contains one parameter 'alarmOnly'.  
// When 'alarmOnly' is TRUE (1) the find alarm command 
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state. 
//
// 'doReset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search. 
// 'alarmOnly' - TRUE (1) the find alarm command 0xEC is 
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's 
//                       Serial Number placed in the global SerialNum
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
bool oneWireFindNext (bool doReset, bool alarmOnly)
{
    uint8_t romBitIndex = 1;
    uint8_t romByteIndex = 0;
    uint8_t bitMask = 1;
    uint8_t attempt = 0;
    uint8_t discrepMarker = 0;
    bool outBit = 0;
    bool result = FALSE;
    uint8_t lastcrc8 = 0;

    // clear serial number buffer for new search
    memset(SerialNum, 0, 8);

    // if the last call was the last one 
    if (LastDevice)
    {
        // reset the search
        LastDiscrepancy = 0;
        LastDevice = FALSE;
        LastFamilyDiscrepancy = 0;  

        return FALSE;
    }

    // check if reset bus was requested
    if (doReset)
    {
        // reset the 1-Wire bus
        // if there is no presence pulse there is nothing to do here, return FALSE
        if (!oneWireTouchReset())
        {
            // reset the search
            LastDiscrepancy = 0;
            LastFamilyDiscrepancy = 0;
            return FALSE;
        }
    }
 
    // send search command
    if (alarmOnly)
    {
        // conditional search command (devices in alarm condition)
        oneWireTouchByte(COND_SEARCH_ROM);
    }
    else
    {
        // ROM search command
        oneWireTouchByte(SEARCH_ROM);
    }

    do
    {
        attempt = 0;

        if (oneWireTouchBit(TRUE) == 1)
        {
            attempt = 2;
        }

        if (oneWireTouchBit(TRUE) == 1)
        {
            attempt |= 1;
        }

        if (attempt == 3)
        {
            // no devices present, done here
            break;
        }
        else
        {
            if (attempt > 0)
            {
                // all connected devices reply with 0 or 1
                // write bit to perform search
                outBit = attempt >> 1;
            }
            else
            {
                if (romBitIndex < LastDiscrepancy)
                {
                    outBit = ((SerialNum[romByteIndex] & bitMask) > 0);
                }
                else
                {
                    // set to 1 if it's the same, otherwise 0
                    outBit = (romBitIndex == LastDiscrepancy);
                }

                // on 0 record position on bitMask
                if (outBit == 0)
                {
                    discrepMarker = romBitIndex;
                }
            }

            // isolate bit in ROM[n] with bitMask
            if (outBit == 1)
            {
                SerialNum[romByteIndex] |= bitMask;
            }
            else
            {
                SerialNum[romByteIndex] &= ~bitMask;
            }

            // ROM search write
            oneWireTouchBit(outBit);

            romBitIndex++;

            bitMask = bitMask << 1;
            
            // if the mask has reached 0 then go for a new ROM
            if (bitMask == 0)
            { 
                // reset mask and perform CRC8
                lastcrc8 = doCrc8(lastcrc8, SerialNum[romByteIndex]);

                romByteIndex++;
                bitMask++;
            }
        }

    }
    while (romByteIndex < 8);  // loop until we have all ROM bytes

    if (romBitIndex < (65 || lastcrc8))
    {
        // search was unsuccessful reset the last discrepancy
        LastDiscrepancy = 0;
    }
    else
    {
        // search was successful: set last discrepancy, device and result
        LastDiscrepancy = discrepMarker;
        LastDevice = (LastDiscrepancy == 0);
        
        // search isn't completed there are more devices present in the bus
        result = TRUE;
    }

    return result;
}   

#endif /* HAL_USE_STM32_ONEWIRE */
