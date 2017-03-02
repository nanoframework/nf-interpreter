//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>

#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#if (HAL_USE_SERIAL_USB == TRUE)
#include "usbcfg.h"
#endif

WP_Message inboundMessage;

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// The functions bellow are the ones that need to be ported to new channels/HALs when required
// These are to be considered as a reference implementations when working on new ports
// 
// This reference implementation provides communication through:
// - serial port (UART/USART) 
// - serial over USB (USB CDC class device)
//
////////////////////////////////////////////////////////////////////////////////////////////////

#if (HAL_USE_SERIAL_USB == TRUE)

bool WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;

    // sanity check for request of 0 size
    if(*size)
    {
        //////////////////////////////////////////////////////////
        //               PORTING CHANGE REQUIRED HERE           //
        //////////////////////////////////////////////////////////
        // change here to read (size) bytes from the input stream
        // preferably with read timeout and being able to check 
        // if the requested number of bytes was actually read
        //////////////////////////////////////////////////////////
        
        // read from serial stream 
        volatile size_t read = chSequentialStreamRead((BaseSequentialStream *)&SDU1, ptr, *size);

        ptr  += read;
        *size -= read;

        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}
#elif (HAL_USE_SERIAL == TRUE)

bool WP_ReceiveBytes(uint8_t* ptr, uint16_t* size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;

    // sanity check for request of 0 size
    if(*size)
    {
        //////////////////////////////////////////////////////////
        //               PORTING CHANGE REQUIRED HERE           //
        //////////////////////////////////////////////////////////
        // change here to read (size) bytes from the input stream
        // preferably with read timeout and being able to check 
        // if the requested number of bytes was actually read
        //////////////////////////////////////////////////////////
        
        // non blocking read from serial port with 100ms timeout
        volatile size_t read = sdReadTimeout(&SD2, ptr, *size, MS2ST(100));

        ptr  += read;
        *size -= read;

        // check if the requested read matches the actual read count
        return (requestedSize == read);
    }

    return true;
}

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif

#if (HAL_USE_SERIAL_USB == TRUE)

bool WP_TransmitMessage(WP_Message* message)
{
    ///////////////////////////////////////////////////////////
    //              PORTING CHANGE REQUIRED HERE             //
    ///////////////////////////////////////////////////////////
    // change here to write (size) bytes to the output stream
    // preferably with timeout and being able to check 
    // if the write was sucessfull or at least buffered
    //////////////////////////////////////////////////////////

    // write header to output stream
    if(chSequentialStreamWrite((BaseSequentialStream *)&SDU1, (const uint8_t *)&message->m_header, sizeof(message->m_header)) != sizeof(message->m_header)) return false;

    // if there is anything on the payload send it to the output stream
    if(message->m_header.m_size && message->m_payload)
    {
        ///////////////////////////////////////////////////////////
        //              PORTING CHANGE REQUIRED HERE             //
        ///////////////////////////////////////////////////////////
        // see description above
        //////////////////////////////////////////////////////////
        if(chSequentialStreamWrite((BaseSequentialStream *)&SDU1, message->m_payload, message->m_header.m_size ) != message->m_header.m_size) return false;
    }

    return true;    
}
#elif (HAL_USE_SERIAL == TRUE)

bool WP_TransmitMessage(WP_Message* message)
{
    ///////////////////////////////////////////////////////////
    //              PORTING CHANGE REQUIRED HERE             //
    ///////////////////////////////////////////////////////////
    // change here to write (size) bytes to the output stream
    // preferably with timeout and being able to check 
    // if the write was sucessfull or at least buffered
    //////////////////////////////////////////////////////////

    // write header to output stream
    if(sdWrite(&SD2, (const uint8_t *)&message->m_header, sizeof(message->m_header)) != sizeof(message->m_header)) return false;

    // if there is anything on the payload send it to the output stream
    if(message->m_header.m_size && message->m_payload)
    {
        ///////////////////////////////////////////////////////////
        //              PORTING CHANGE REQUIRED HERE             //
        ///////////////////////////////////////////////////////////
        // see description above
        //////////////////////////////////////////////////////////
        if(sdWrite(&SD2, message->m_payload, message->m_header.m_size ) != message->m_header.m_size) return false;
    }

    return true;    
}

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif

void WP_CheckAvailableIncomingData()
{
    //////////////////////////////////////////////////////
    //            PORTING CHANGE REQUIRED HERE          //
    //////////////////////////////////////////////////////
    // change here the check for existing data to be read
    //////////////////////////////////////////////////////

#if (HAL_USE_SERIAL_USB == TRUE)

    // only makes sense to check the stream if USB state is active (meaning it's connected to an USB host)
    if (SDU1.config->usbp->state == USB_ACTIVE) {

        // check if there is any data waiting to be read
        if(!sdGetWouldBlock(&SDU1))
        {
            WP_Message_Initialize(&inboundMessage);
            WP_Message_PrepareReception(&inboundMessage);

            WP_Message_Process(&inboundMessage);
        }
    }

#elif (HAL_USE_SERIAL == TRUE)

    if(!sdGetWouldBlock(&SD2))
    {
        WP_Message_Initialize(&inboundMessage);
        WP_Message_PrepareReception(&inboundMessage);

        WP_Message_Process(&inboundMessage);
    }

#else
#error "Wire Protocol needs a transport. Please make sure that HAL_USE_SERIAL and/or HAL_USE_SERIAL_USB are set to TRUE in 'halconf.h'"
#endif
    
}

