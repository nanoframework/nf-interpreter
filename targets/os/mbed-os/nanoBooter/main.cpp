//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "mbed.h"
#include <string.h>
#include <WireProtocol.h>


DigitalOut led1(LED1, 0);   // Initially OFF


//
// Serial that does not implement locking, so it can be used from within interrupt handler.
//
class MySerial : public SerialBase
{
public:
    MySerial(PinName tx, PinName rx, int baud) : SerialBase(tx, rx, baud)
    {
    }

    int getc()
    {
        return _base_getc();
    }
    
    int putc(int c)
    {
        return _base_putc(c);
    }

    int readable()
    {
        int ret = serial_readable(&_serial);
        return ret;
    }

    void lock() { }

    void unlock() { }
};


MySerial serial(USBTX, USBRX, 115200);

const int32_t signalDataIn = 0x10;

const int BufferSize = 255;

uint8_t rxBuffer[BufferSize + 1];

volatile int rxIn  = 0;
volatile int rxOut = 0;

WP_Message wpMessage;

Thread rt;

extern unsigned int SUPPORT_ComputeCRC(const void* rgBlock, int nLength, unsigned int crc);

class Receiver
{
public:
    Receiver(WP_Message* msg)
    {
        this->msg = msg;
        this->msg->Initialize(NULL);
        
        m_lastOutboundMessage = 0;
    }
    
public:
    void receive()
    {
        msg->PrepareReception();

        while(true)
        {
            // Note: For reason unknown, the signal is always set during startup (?)
            Thread::signal_wait(signalDataIn);

            process();
        }
    }

private:
    bool process()
    {
        //
        // UNDONE: FIXME: Copy of WireProtocol.cpp code
        //
        while(true)
        {
            switch(msg->m_rxState)
            {
                case WP_Message::ReceiveState::Idle:
                    return true;
                
                case WP_Message::ReceiveState::Initialize:
                    msg->m_rxState = WP_Message::ReceiveState::WaitingForHeader;
                    msg->m_pos = (unsigned char*)&msg->m_header;
                    msg->m_size = sizeof(msg->m_header);
                    break;

                case WP_Message::ReceiveState::WaitingForHeader:
                    if(!readData())
                    {
                        return true;
                    }
                    if(syncToMessageStart())
                    {
                        msg->m_rxState = WP_Message::ReceiveState::ReadingHeader;
                    }
                    break;
                    
                case WP_Message::ReceiveState::ReadingHeader:
                    if(readData())
                    {
                        msg->m_rxState = WP_Message::ReceiveState::CompleteHeader;
                    }
                    break;

                case WP_Message::ReceiveState::CompleteHeader:
                    // UNDONE: VerifyHeader
                    //if(processHeader())
                    {
                        if(msg->m_header.m_size)
                        {
                            // UNDONE: Receive payload
                        }
                        else
                        {
                            msg->m_rxState = WP_Message::ReceiveState::CompletePayload;
                        }
                    }
                    // UNDONE: fBadPacket
                    break;

                case WP_Message::ReceiveState::CompletePayload:
                    // UNDONE: VerifyPayload

                    processPayload();
                    // else ReplyBadPacket

                    msg->m_rxState = WP_Message::ReceiveState::Initialize;
                    break;

                default:
                    // Unknown state
                    return false;
            }
        }
    }

    bool syncToMessageStart()
    {
        int len;
        while(true)
        {
            len = sizeof(msg->m_header) - msg->m_size;
            if(len <= 0)
            {
                break;
            }

            size_t lenCmp = min(len, sizeof(msg->m_header.m_signature));

            if(memcmp(&msg->m_header, MARKER_DEBUGGER_V1, lenCmp) == 0)
            {
                break;
            }
            if(memcmp(&msg->m_header, MARKER_PACKET_V1, lenCmp) == 0)
            {
                break;
            }

            unsigned char* buff = (unsigned char*)&msg->m_header;

            memmove(buff, &buff[1], len - 1);

            msg->m_pos--;
            msg->m_size++;
        }
        return (len >= sizeof(msg->m_header.m_signature));
    }


    bool readData()
    {
        // TODO: Disable interrupts
        while(msg->m_size > 0)
        {
            if(rxIn == rxOut)   // Buffer is empty
            {
                break;
            }
            *msg->m_pos++ = rxBuffer[rxOut];
            msg->m_size--;

            rxOut = (rxOut + 1) % BufferSize;
        }
        return msg->m_size == 0;
    }

    
    void processPayload()
    {
        WP_Message reply;
        
        reply.Initialize(NULL);
        reply.m_header.m_flags |= WP_Flags::c_ACK;
        reply.m_header.m_flags |= WP_Flags::c_NonCritical;

        uint32_t pingReplyPayload[2] = 
        {
            //public const uint c_Ping_Source_CLR = 0x00000000;
            //public const uint c_Ping_Source_Booter = 0x00000001;
            //public const uint c_Ping_Source_Host = 0x00000002;
            
            1, // Source Booter

            //public const uint c_Ping_DbgFlag_Stop       = 0x00000001;
            //public const uint c_Ping_DbgFlag_BigEndian  = 0x02000002;
            //public const uint c_Ping_DbgFlag_AppExit    = 0x00000004;
            0, // Flags
        };


        // UNDONE: PrepareReply() - m_parent is NULL
        memcpy(reply.m_header.m_signature, MARKER_PACKET_V1, sizeof(reply.m_header.m_signature));
        reply.m_header.m_cmd = msg->m_header.m_cmd;
        reply.m_header.m_seq = m_lastOutboundMessage++;
        reply.m_header.m_seqReply = msg->m_header.m_seq;
        reply.m_header.m_flags |= WP_Flags::c_Reply;
        reply.m_header.m_size = sizeof(pingReplyPayload);
        reply.m_payload = (unsigned char*)pingReplyPayload;

        reply.m_header.m_crcData = SUPPORT_ComputeCRC(reply.m_payload, reply.m_header.m_size, 0);

        //
        // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
        //
        reply.m_header.m_crcHeader = 0;
        reply.m_header.m_crcHeader = SUPPORT_ComputeCRC((unsigned char*)&reply.m_header, sizeof(reply.m_header), 0);
        

        uint8_t* p = (uint8_t*)&reply.m_header;
        for(int i = 0; i < sizeof(reply.m_header); i++)
        {
            serial.putc(p[i]);
        }
        for(int i = 0; i < reply.m_header.m_size; i++)
        {
            serial.putc(reply.m_payload[i]);
        }
        p=p;
    }

    WP_Message* msg;
    uint8_t m_buffer[256];
    int m_lastOutboundMessage;
};

Receiver rec(&wpMessage);



void rxInterrupt()
{
    // Note: you need to actually read from the serial to clear the RX interrupt
    while(serial.readable() && (((rxIn + 1) % BufferSize) != rxOut))
    {
        rxBuffer[rxIn] = serial.getc();
        rxIn = (rxIn + 1) % BufferSize;
    }

    rt.signal_set(signalDataIn);
}

/////////////////////////////////////////////////////////////////////////////
//
// Application entry point
//

int main()
{
    // TODO: if(isBootloaderRequired(timeout)

    // TODO: Configure transport (USART, USB, TCP/IP)
    serial.attach(callback(rxInterrupt), Serial::RxIrq);

    // TODO: Replace with nanoBooter.Start()
    rt.start(callback(&rec, &Receiver::receive));

    while(true)
    {
        Thread::wait(10000);
    }
}

