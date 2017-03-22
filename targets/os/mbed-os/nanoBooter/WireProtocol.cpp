//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol.h>

#define TRACE0(f, msg, ...)
#define TRACE(f, msg, ...)


unsigned int SUPPORT_ComputeCRC(const void* rgBlock, int nLength, unsigned int crc);


/////////////////////////////////////////////////////////////////////////////
//
// WP_Message
//

void WP_Message::Initialize(WP_Controller* parent)
{
    m_parent = parent;

    memset(&m_header, 0, sizeof(m_header));
    m_payload = NULL;
    m_pos     = NULL;
    m_size    = 0;
    m_payloadTicks = 0;
    m_rxState = ReceiveState::Idle;
}

void WP_Message::PrepareReception()
{
    m_rxState = ReceiveState::Initialize;
}

void WP_Message::PrepareRequest(unsigned int cmd, unsigned int flags, unsigned int payloadSize, unsigned char* payload)
{
    memcpy(m_header.m_signature, m_parent->m_szMarker ? m_parent->m_szMarker : MARKER_PACKET_V1, sizeof(m_header.m_signature));

    m_header.m_crcData   = SUPPORT_ComputeCRC(payload, payloadSize, 0);
    m_header.m_cmd       = cmd;
    m_header.m_seq       = m_parent->m_lastOutboundMessage++;
    m_header.m_seqReply  = 0;
    m_header.m_flags     = flags;
    m_header.m_size      = payloadSize;
    m_payload            = payload;

    //
    // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
    //
    m_header.m_crcHeader = 0;
    m_header.m_crcHeader = SUPPORT_ComputeCRC((unsigned char*)&m_header, sizeof(m_header), 0);
}

void WP_Message::PrepareReply(const WP_Packet& req, unsigned int flags, unsigned int payloadSize, unsigned char* payload)
{
    memcpy(m_header.m_signature, m_parent->m_szMarker ? m_parent->m_szMarker : MARKER_PACKET_V1, sizeof(m_header.m_signature));

    m_header.m_crcData   = SUPPORT_ComputeCRC(payload, payloadSize, 0);
    m_header.m_cmd       = req.m_cmd;
    m_header.m_seq       = m_parent->m_lastOutboundMessage++;
    m_header.m_seqReply  = req.m_seq;
    m_header.m_flags     = flags | WP_Flags::c_Reply;
    m_header.m_size      = payloadSize;
    m_payload            = payload;

    //
    // The CRC for the header is computed setting the CRC field to zero and then running the CRC algorithm.
    //
    m_header.m_crcHeader = 0;
    m_header.m_crcHeader = SUPPORT_ComputeCRC((unsigned char*)&m_header, sizeof(m_header), 0);
}

void WP_Message::SetPayload(unsigned char* payload)
{
    m_payload = payload;
}

void WP_Message::Release()
{
    if(m_payload)
    {
        m_parent->m_app->Release(m_parent->m_state, this);
        m_payload = NULL;
    }
}

bool WP_Message::VerifyHeader()
{
    unsigned int crc = m_header.m_crcHeader;
    m_header.m_crcHeader = 0;
    unsigned int computedCrc = SUPPORT_ComputeCRC((unsigned char*)&m_header, sizeof(m_header), 0);
    m_header.m_crcHeader = crc;

    if(computedCrc != crc)
    {
        TRACE(TRACE_ERRORS, "Header CRC check failed: computed: 0x%08X; got: 0x%08X\n", computedCrc, m_header.m_crcHeader);
        return false;
    }
    return true;
}

bool WP_Message::VerifyPayload()
{
    if(m_payload == NULL && m_header.m_size)
    {
        return false;
    }

    unsigned int computedCrc = SUPPORT_ComputeCRC(m_payload, m_header.m_size, 0);
    if(computedCrc != m_header.m_crcData)
    {
        TRACE(TRACE_ERRORS, "Payload CRC check failed: computed: 0x%08X; got: 0x%08X\n", computedCrc, m_header.m_crcData);
        return false;
    }
    return true;
}

void WP_Message::ReplyBadPacket(unsigned int flags)
{
    WP_Message msg;
    msg.Initialize(m_parent);
    msg.PrepareRequest(0, WP_Flags::c_NonCritical | WP_Flags::c_NACK | flags, 0, NULL);

    m_parent->SendProtocolMessage(msg);
}

bool WP_Message::Process()
{
    unsigned char* buf = (unsigned char*)&m_header;
    int len;

    while(true)
    {
        switch(m_rxState)
        {
            case ReceiveState::Idle:
                TRACE0(TRACE_STATE, "RxState==IDLE\n");
                return true;

            case ReceiveState::Initialize:
                TRACE0(TRACE_STATE, "RxState==INIT\n");
                Release();

                m_rxState = ReceiveState::WaitingForHeader;
                m_pos     = (unsigned char*)&m_header;
                m_size    = sizeof(m_header);
                break;

            case ReceiveState::WaitingForHeader:
                TRACE0(TRACE_STATE, "RxState==WaitForHeader\n");
                if(m_parent->m_phy->ReceiveBytes(m_parent->m_state, m_pos, m_size) == false)
                {
                    TRACE0(TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                    return true;
                }

                //
                // Synch to the start of a message.
                //
                while(true)
                {
                    len = sizeof(m_header) - m_size;
                    if(len <= 0)
                    {
                        break;
                    }

                    size_t lenCmp = min(len, sizeof(m_header.m_signature));

                    if(memcmp(&m_header, MARKER_DEBUGGER_V1, lenCmp) == 0)
                    {
                        break;
                    }
                    if(memcmp(&m_header, MARKER_PACKET_V1, lenCmp) == 0)
                    {
                        break;
                    }

                    memmove(&buf[0], &buf[1], len-1);

                    m_pos--;
                    m_size++;
                }

                if(len >= sizeof(m_header.m_signature))
                {
                    m_rxState = ReceiveState::ReadingHeader;
                }
                break;

            case ReceiveState::ReadingHeader:
                TRACE0(TRACE_STATE, "RxState==ReadingHeader\n");
                if(m_parent->m_phy->ReceiveBytes(m_parent->m_state, m_pos, m_size) == false)
                {
                    TRACE0(TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                    return true;
                }

                if(m_size == 0)
                {
                    m_rxState = ReceiveState::CompleteHeader;
                }
                break;

            case ReceiveState::CompleteHeader:
            {
                TRACE0(TRACE_STATE, "RxState=CompleteHeader\n");

                bool fBadPacket = true;

                if(VerifyHeader())
                {
                    TRACE(TRACE_HEADERS, "RXMSG: 0x%08X, 0x%08X, 0x%08X\n", m_header.m_cmd, m_header.m_flags, m_header.m_size);
                    if(m_parent->m_app->ProcessHeader(m_parent->m_state, this))
                    {
                        fBadPacket = false;

                        if(m_header.m_size)
                        {
                            if(m_payload == NULL) // Bad, no buffer...
                            {
                                m_rxState = ReceiveState::Initialize;
                            }
                            else
                            {
                                // FIXME: m_payloadTicks = HAL_Time_CurrentSysTicks();
                                m_rxState = ReceiveState::ReadingPayload;
                                m_pos     = (unsigned char*)m_payload;
                                m_size    = m_header.m_size;
                            }
                        }
                        else
                        {
                            m_rxState = ReceiveState::CompletePayload;
                        }
                    }
                }

                if(fBadPacket)
                {
                    if((m_header.m_flags & WP_Flags::c_NonCritical) == 0)
                    {
                        ReplyBadPacket(WP_Flags::c_BadHeader);
                    }

                    m_rxState = ReceiveState::Initialize;
                }
            }
            break;

            case ReceiveState::ReadingPayload:
            {
                TRACE0(TRACE_STATE, "RxState=ReadingPayload\n");

                // FIXME: unsigned __int64 curTicks = HAL_Time_CurrentSysTicks();

                // If the time between consecutive payload bytes exceeds the timeout threshold then assume that
                // the rest of the payload is not coming. Reinitialize to synch on the next header.

                // FIXME: if(HAL_Time_SysTicksToTime(curTicks - m_payloadTicks) < (unsigned __int64)c_PayloadTimeout)
                {
                    // FIXME: m_payloadTicks = curTicks;

                    if(m_parent->m_phy->ReceiveBytes(m_parent->m_state, m_pos, m_size) == false)
                    {
                        TRACE0(TRACE_NODATA, "ReceiveBytes returned false - bailing out\n");
                        return true;
                    }

                    if(m_size == 0)
                    {
                        m_rxState = ReceiveState::CompletePayload;
                    }
                }
                // FIXME: else
                //{
                //    TRACE0(TRACE_ERRORS, "RxError: Payload InterCharacterTimeout exceeded\n");
                //    m_rxState = ReceiveState::Initialize;
                //}
            }
            break;

            case ReceiveState::CompletePayload:
                TRACE0(TRACE_STATE, "RxState=CompletePayload\n");
                if(VerifyPayload() == true)
                {
                    m_parent->m_app->ProcessPayload(m_parent->m_state, this);
                }
                else
                {
                    ReplyBadPacket(WP_Flags::c_BadPayload);
                }

                m_rxState = ReceiveState::Initialize;
                break;


            default:
                TRACE0(TRACE_ERRORS, "RxState=UNKNOWN!!\n");
                return false;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// WP_Controller
//

void WP_Controller::Initialize(const char* szMarker, const WP_PhysicalLayer* phy, const WP_ApplicationLayer* app, void* state)
{
    m_szMarker = szMarker;
    m_phy = phy;
    m_app = app;
    m_state = state;
    m_lastOutboundMessage = 0;

    memset(&m_inboundMessage, 0, sizeof(m_inboundMessage));

    m_inboundMessage.Initialize(this);
    m_inboundMessage.PrepareReception();
}

bool WP_Controller::AdvanceState()
{
    return m_inboundMessage.Process();
}

bool WP_Controller::SendProtocolMessage(const WP_Message& msg)
{
    TRACE(TRACE_HEADERS, "TXMSG: 0x%08X, 0x%08X, 0x%08X\n", msg.m_header.m_cmd, msg.m_header.m_flags, msg.m_header.m_size);
    return m_phy->TransmitMessage(m_state, &msg);
}

bool WP_Controller::SendProtocolMessage(unsigned int cmd, unsigned int flags, unsigned int payloadSize, unsigned char* payload)
{
    WP_Message msg;
    msg.Initialize(this);
    msg.PrepareRequest(cmd, flags, payloadSize, payload);

    return SendProtocolMessage(msg);
}

/////////////////////////////////////////////////////////////////////////////
//
// CRC 32 table for use under ZModem protocol, IEEE 802
// G(x) = x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
//
static const unsigned int c_CRCTable[256] =
{
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
    0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
    0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
    0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
    0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
    0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
    0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
    0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
    0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
    0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
    0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
    0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
    0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
    0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
    0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
    0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
    0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
    0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
    0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
    0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
    0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
    0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
    0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
    0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
    0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
    0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
    0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
    0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
    0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
    0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

unsigned int SUPPORT_ComputeCRC(const void* rgBlock, int nLength, unsigned int crc)
{
    const unsigned char* ptr = (const unsigned char*)rgBlock;

    while(nLength-- > 0)
    {
        crc = c_CRCTable[((crc >> 24) ^ (*ptr++)) & 0xFF] ^ (crc << 8);
    }

    return crc;
}

