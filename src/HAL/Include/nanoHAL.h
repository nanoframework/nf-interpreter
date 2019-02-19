//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_H_
#define _NANOHAL_H_ 1

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <nanoWeak.h>
#include <nanoHAL_v2.h>

#if defined(PLATFORM_EMULATED_FLOATINGPOINT)
/***************************************************/
// Keep in sync with the nanoCLR_Runtime__HeapBlock.h

#define HAL_FLOAT_SHIFT         10
#define HAL_FLOAT_PRECISION     1000

#define HAL_DOUBLE_SHIFT        16
#define HAL_DOUBLE_PRECISION    10000

/************************************************/
#else
#include <math.h>
#endif



#include <nanoHAL_Power.h>
#include <nanoHAL_Time.h>
#include <targetHAL.h>
#include <nanoHAL_Types.h>
#include <nanoHAL_ReleaseInfo.h>

//#if !defined(_WIN32) && !defined(FIQ_SAMPLING_PROFILER) && !defined(HAL_REDUCESIZE) && defined(PROFILE_BUILD)
//#define ENABLE_NATIVE_PROFILER
//#endif

//#include "..\pal\Diagnostics\Native_Profiler.h"

#define NATIVE_PROFILE_CLR_DEBUGGER()
#define NATIVE_PROFILE_CLR_UTF8_DECODER()
#define NATIVE_PROFILE_CLR_CORE()
#define NATIVE_PROFILE_CLR_MESSAGING()
#define NATIVE_PROFILE_CLR_SERIALIZATION()
#define NATIVE_PROFILE_CLR_NETWORK()
#define NATIVE_PROFILE_CLR_I2C()
#define NATIVE_PROFILE_CLR_DIAGNOSTICS()
#define NATIVE_PROFILE_CLR_HARDWARE()
#define NATIVE_PROFILE_CLR_GRAPHICS()
#define NATIVE_PROFILE_CLR_STARTUP()
#define NATIVE_PROFILE_CLR_HEAP_PERSISTENCE()
#define NATIVE_PROFILE_CLR_IOPORT()
#define NATIVE_PROFILE_CLR_IO()


#if defined(_MSC_VER)

#define ADS_PACKED
#define GNU_PACKED





#elif defined(__GNUC__)


#define ADS_PACKED
#define GNU_PACKED  __attribute__((packed))
























































#else
!ERROR
#endif

//--//














//--//



//--//







//--//












































#define TRANSPORT_SHIFT             8
#define TRANSPORT_MASK              (0xFF << TRANSPORT_SHIFT)
#define PORT_NUMBER_MASK            0x00FF

// Macro to extract the transport type from a COM_HANDLE
#define ExtractTransport(x)         ((unsigned int)(x) & TRANSPORT_MASK)

// Macro to extract well-known system event flag ids from a COM_HANDLE
#define ExtractEventFromTransport(x) (ExtractTransport(x) == USART_TRANSPORT     ? SYSTEM_EVENT_FLAG_COM_IN: \
                                      ExtractTransport(x) == SOCKET_TRANSPORT    ? SYSTEM_EVENT_FLAG_SOCKET: \
                                      ExtractTransport(x) == GENERIC_TRANSPORT   ? SYSTEM_EVENT_FLAG_GENERIC_PORT: \
                                      ExtractTransport(x) == DEBUG_TRANSPORT     ? SYSTEM_EVENT_FLAG_DEBUGGER_ACTIVITY: \
                                      ExtractTransport(x) == MESSAGING_TRANSPORT ? SYSTEM_EVENT_FLAG_MESSAGING_ACTIVITY: \
                                      0) \

#define USART_TRANSPORT             (1 << TRANSPORT_SHIFT)
//#define COM_NULL                    ((COM_HANDLE)(USART_TRANSPORT))

#define USB_TRANSPORT               (2 << TRANSPORT_SHIFT)
//#define USB_CONTROLLER_SHIFT        5
//#define USB_CONTROLLER_MASK         0xE0
//#define USB_STREAM_MASK             0x00FF
//#define USB_STREAM_INDEX_MASK       0x001F

#define SOCKET_TRANSPORT            (3 << TRANSPORT_SHIFT)
#define COM_SOCKET_DBG              ((COM_HANDLE)(SOCKET_TRANSPORT + 1))

#define DEBUG_TRANSPORT             (4 << TRANSPORT_SHIFT)







#define MESSAGING_TRANSPORT         (7 << TRANSPORT_SHIFT)

#define GENERIC_TRANSPORT           (8 << TRANSPORT_SHIFT)











































// Creates a COM_HANDLE value for a platform specific port number
#define ConvertCOM_DebugHandle(x)    ((COM_HANDLE)((x) + DEBUG_TRANSPORT     + 1))

// Extracts a Socket transport port id from a SOCKET_TRASNPORT COM_HANDLE
#define ConvertCOM_SockPort(x)      (((x) & PORT_NUMBER_MASK) - 1)

typedef unsigned int FLASH_WORD;


















































































































































































































































typedef void (*LOGGING_CALLBACK)(const char* text);































// //--//

// //--//




//--//














































//--//














//--//











































template <class T> class HAL_DblLinkedList;

template <class T> class HAL_DblLinkedNode
{
    T* m_nextNode;
    T* m_prevNode;

    friend class HAL_DblLinkedList<T>;

public:
    void Initialize()
    {
        m_nextNode = NULL;
        m_prevNode = NULL;
    }

    T* Next() const { return m_nextNode; }
    T* Prev() const { return m_prevNode; }

    void SetNext( T* next ) { m_nextNode = next; }
    void SetPrev( T* prev ) { m_prevNode = prev; }

    bool IsLinked() const { return m_nextNode != NULL; }

    //--//

    void RemoveFromList()
    {
        T* next = m_nextNode;
        T* prev = m_prevNode;

        if(prev) prev->m_nextNode = next;
        if(next) next->m_prevNode = prev;
    }

    void Unlink()
    {
        T* next = m_nextNode;
        T* prev = m_prevNode;

        if(prev) prev->m_nextNode = next;
        if(next) next->m_prevNode = prev;

        m_nextNode = NULL;
        m_prevNode = NULL;
    }
};

//--//

// The use of offsetof below throwns an "invalid offset warning" because CLR_RT_StackFrame is not POD type 
// C+17 is the first standard that allow this, so until we are using it we have to disable it to keep GCC happy 

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

template <class T> class HAL_DblLinkedList
{
    //
    // Logically, a list starts with a HAL_DblLinkedNode with only the Next() set and ends with a node with only Prev() set.
    // This can be collapsed to have the two nodes overlap.
    //
    T* m_first;
    T* m_null;
    T* m_last;

    //--//

public:
    void Initialize()
    {
        m_first = Tail();
        m_null  = NULL;
        m_last  = Head();
    }

    int NumOfNodes()
    {
        T*  ptr;
        T*  ptrNext;
        int num = 0;

        for(ptr = FirstNode(); (ptrNext = ptr->Next()) != NULL; ptr = ptrNext)
        {
            num++;
        }

        return num;
    }

    //--//

    T* FirstNode() const { return m_first          ; }
    T* LastNode () const { return m_last           ; }
    bool           IsEmpty  () const { return m_first == Tail(); }

    T* FirstValidNode() const { T* res = m_first; return res->Next() ? res : NULL; }
    T* LastValidNode () const { T* res = m_last ; return res->Prev() ? res : NULL; }

    T* Head() const { return (T*)((size_t)&m_first - offsetof(T, m_nextNode)); }
    T* Tail() const { return (T*)((size_t)&m_last  - offsetof(T, m_prevNode)); }

    //--//

private:

    void Insert( T* prev, T* next, T* node )
    {
        node->m_nextNode = next;
        node->m_prevNode = prev;

        next->m_prevNode = node;
        prev->m_nextNode = node;
    }

public:
#if defined(_DEBUG)
    bool Exists( T* searchNode )
    {
        T* node = FirstValidNode();
        while( node != NULL && node != searchNode )
        {
            if (node == node->Next())
            {
                ASSERT(false);
            }
            node = node->Next();
        }
        return (node == NULL? false: true);
    }
#endif

    void InsertBeforeNode( T* node, T* nodeNew )
    {
        if(node && nodeNew && node != nodeNew)
        {
            nodeNew->RemoveFromList();

            Insert( node->Prev(), node, nodeNew );
        }
    }

    void InsertAfterNode( T* node, T* nodeNew )
    {
        if(node && nodeNew && node != nodeNew)
        {
            nodeNew->RemoveFromList();

            Insert( node, node->Next(), nodeNew );
        }
    }

    void LinkAtFront( T* node )
    {
        InsertAfterNode( Head(), node );
    }

    void LinkAtBack( T* node )
    {
        InsertBeforeNode( Tail(), node );
    }

    T* ExtractFirstNode()
    {
        T* node = FirstValidNode();

        if(node) node->Unlink();

        return node;
    }

    T* ExtractLastNode()
    {
        T* node = LastValidNode();

        if(node) node->Unlink();

        return node;
    }
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

//--//

















































































template <typename T> class Hal_Queue_UnknownSize
{
    size_t m_writer;
    size_t m_reader;
    size_t m_size;
    bool   m_full;
    T*     m_data;

public:
    void Initialize( T* data, size_t size )
    {
        m_writer = 0;
        m_reader = 0;
        m_size   = size;
        m_data   = data;
        m_full   = false;
    }

    size_t NumberOfElements()
    {
        if(m_writer < m_reader) return m_size + m_writer - m_reader;
        else if(m_full)         return m_size;
        else                    return m_writer - m_reader;
    }

    bool IsEmpty()
    {
        return (m_writer == m_reader && !m_full);
    }

    bool IsFull()
    {
        return m_full;
    }

    T* operator[](int index)
    {
        if(index < 0 || index >= NumberOfElements()) return NULL;

        return &m_data[(m_reader + index) % m_size];
    }

    T* Push()
    {
        size_t oldWriter  = m_writer;

        if(m_full) return NULL;

        m_writer++;  if(m_writer == m_size) m_writer = 0;

        if(m_writer == m_reader) m_full = true;

        return &m_data[oldWriter];
    }

    T* Peek()
    {
        if(m_writer == m_reader && !m_full) return NULL;

        return &m_data[m_reader];
    }

    T* Pop()
    {
        size_t oldReader = m_reader;

        if(m_reader == m_writer && !m_full) return (T*)NULL;

        m_reader++;  if(m_reader == m_size) m_reader = 0;

        m_full = false;

        return &m_data[oldReader];
    }

    T* Push( size_t &nElements )
    {
        size_t oldWriter  = m_writer;
        size_t max = 0;

        if(m_full || (nElements == 0))
        {
            nElements = 0;
            return NULL;
        }

        if(m_writer < m_reader) max = m_reader - m_writer;
        else                    max = m_size   - m_writer;

        nElements = (max < nElements? max: nElements);

        m_writer += nElements; if(m_writer == m_size) m_writer = 0;

        if(m_writer == m_reader) m_full = true;

        return &m_data[oldWriter];
    }

    T* Pop( size_t &nElements )
    {
        size_t oldReader = m_reader;
        size_t max = 0;

        if(nElements == 0) return NULL;

        if((m_reader == m_writer) && !m_full)
        {
            nElements = 0;
            // reset the reader/writer to maximize push potential
            m_reader  = 0;
            m_writer  = 0;
            return NULL;
        }

        if(m_writer <= m_reader) max = m_size   - m_reader;
        else                     max = m_writer - m_reader;

        nElements = (max < nElements? max: nElements);

        m_reader += nElements; if(m_reader == m_size) m_reader = 0;

        m_full = false;

        return &m_data[oldReader];
    }

    T* Storage() { return m_data; }
};

template<typename T> class HAL_RingBuffer 
{
    size_t _dataSize;
    size_t _size;
    size_t _capacity;
    size_t _write_index;
    size_t _read_index;
    T * _buffer;

public:

    void Initialize(T* data, size_t size)
    {
        _dataSize = sizeof(T);

        _capacity = (size * _dataSize);
        _write_index = 0;
        _read_index = 0;
        _size = 0;

        _buffer = data;
    }

    size_t Capacity() { return (_capacity / _dataSize); }

    size_t Length() { return (_size / _dataSize); }

    // Push a single element to the buffer.
    size_t Push(const T data)
    {
        // check for buffer full
        if(_size == _capacity)
        {
            // buffer full
            return 0;
        }

        T* destination = _buffer;
        destination += _write_index;

        *destination = data;
        _write_index += _dataSize;
        
        // check if we are the end of the capacity
        if (_write_index == _capacity) _write_index = 0;

        // update ring buffer size
        _size += _dataSize;

        return 1;
    }

    // Push N elements to the buffer.
    size_t Push(const T* data, size_t length)
    {
        size_t lengthToWrite = 0;

        // sanity check for 0 length
        if (length == 0) return 0;

        // check for buffer full
        if(_size == _capacity)
        {
            // buffer full
            return 0;
        }

        if( (length * _dataSize) < (_capacity - _size))
        {
            lengthToWrite = (length * _dataSize);
        }
        else
        {
            lengthToWrite = (_capacity - _size);
        }

        // single memcpy
        if (lengthToWrite <= _capacity - _write_index)
        {
            memcpy(_buffer + _write_index, data, lengthToWrite);
            _write_index += lengthToWrite;
           
            // check if we are the end of the capacity
            if (_write_index == _capacity) _write_index = 0;
        }
        // need to memcpy in two chunks
        else
        {
            size_t chunk1Size = _capacity - _write_index;
            memcpy(_buffer + _write_index, data, chunk1Size);

            size_t chunk2Size = lengthToWrite - chunk1Size;
            memcpy(_buffer, data + chunk1Size, chunk2Size);

            _write_index = chunk2Size;
        }

        // update ring buffer size
        _size += lengthToWrite;

        return (lengthToWrite / _dataSize);
    }

    // Pop N elements from ring buffer returning them in the data argument.
    size_t Pop(T* data, size_t length)
    {
        size_t lengthToRead = 0;

        // sanity check for 0 length
        if (length == 0) return 0;

        // check for buffer empty
        if(_size == 0)
        {
            return 0;
        }

        lengthToRead = (length * _dataSize);

        // can read in a single memcpy
        if (lengthToRead <= _capacity - _read_index)
        {
            memcpy(data, _buffer + _read_index, lengthToRead);
            _read_index += lengthToRead;

            // check if we are at end of capacity
            if (_read_index == _capacity) _read_index = 0;
        }
        // need to memcpy in two steps
        else
        {
            size_t chunk1Size = _capacity - _read_index;
            memcpy(data, _buffer + _read_index, chunk1Size);

            size_t chunk2Size = lengthToRead - chunk1Size;
            memcpy(data + chunk1Size, _buffer, chunk2Size);

            _read_index = chunk2Size;
        }

        // update ring buffer size
        _size -= lengthToRead;

        // check for optimization to improve sequential push
        // buffer has to be empty and read and write indexes coincide
        if(_size == 0 && (_write_index == _read_index))
        {
            // reset the read/write index
            _write_index = 0;
            _read_index = 0;
        }

        return (lengthToRead / _dataSize);
    }

    // Pop N elements from ring buffer. The elements are not actually returned, just popped from the buffer.
    size_t Pop(size_t length)
    {
        size_t lengthToRead = 0;

        // sanity check for 0 length
        if (length == 0) return 0;

        // check for buffer empty
        if(_size == 0)
        {
            return 0;
        }

        lengthToRead = (length * _dataSize);

        // can read in a single memcpy
        if (lengthToRead <= _capacity - _read_index)
        {
            _read_index += lengthToRead;

            // check if we are at end of capacity
            if (_read_index == _capacity) _read_index = 0;
        }
        // need to memcpy in two steps
        else
        {
            size_t chunk1Size = _capacity - _read_index;
            size_t chunk2Size = lengthToRead - chunk1Size;
            _read_index = chunk2Size;
        }

        // update ring buffer size
        _size -= lengthToRead;

        // check for optimization to improve sequential push
        // buffer has to be empty and read and write indexes coincide
        if(_size == 0 && (_write_index == _read_index))
        {
            // reset the read/write index
            _write_index = 0;
            _read_index = 0;
        }

        return (lengthToRead / _dataSize);
    }

    void OptimizeSequence()
    {
        // no elements, so there is nothing to optimize
        if(_size == 0) return;
     
        // read index is already at index 0, so there is nothing to optimize
        if(_read_index == 0) return;

        // can move data in a single memcpy
        if (_read_index < _write_index)
        {
            // buffer looks like this
            // |...xxxxx.....|
            memcpy(_buffer, _buffer + _read_index, _size);
        }
        // need to move data in two steps
        else
        {
            // buffer looks like this
            // |xxxx......xxxxxx|
            
            // store size of tail
            size_t tailSize = _write_index - (1 * _dataSize);

            // 1st move tail to temp buffer (need to malloc first)
            T* tempBuffer = (T*)platform_malloc(tailSize);
            
            memcpy(tempBuffer, _buffer, tailSize);
            
            // store size of remaining buffer
            size_t headSize = _capacity - _read_index;

            // 2nd move head to start of buffer
            memcpy(_buffer, _buffer + _read_index, headSize);

            // 3rd move temp buffer after head
            memcpy(_buffer + headSize, tempBuffer, tailSize);

            // free memory
            platform_free(tempBuffer);
        }

        // adjust indexes
        _read_index = 0;
        _write_index = _size;
    }

    T* Reader() { return _buffer + _read_index; }
};

//--//

//#include <..\Initialization\MasterConfig.h>






//--//


































































































































// hal cleanup for CLR reboot

void nanoHAL_Initialize();
void nanoHAL_Uninitialize();

void HAL_EnterBooterMode();

typedef void (*ON_SOFT_REBOOT_HANDLER)(void);

void HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler);

//--//

















//--//



//--//






































































































































































































//--//

extern bool g_fDoNotUninitializeDebuggerPort;

//--//

#include <nanoPAL_Network.h>
#include <nanoPAL.h>
//#include <drivers.h>

/////////////////////////////////////////////////////////////////////
//
// Chipset
//

//// boot
//#include <CPU_BOOT_decl.h>
//
//// Cache driver
//#include <CPU_MMU_decl.h>
//
//// Cache driver
//#include <CPU_CACHE_decl.h>
//
//// Gp I/O driver
//#include <CPU_INTC_decl.h>
//
//// Gp I/O driver
//#include <CPU_GPIO_decl.h>
//


















//
//// SPI driver
//#include <CPU_SPI_decl.h>
//
//// External bus interface driver
//#include <CPU_EBIU_decl.h>
//
//// Power control unit
//#include <CPU_PCU_decl.h>
//
//// Clock management unit driver
//#include <CPU_CMU_decl.h>
//
//// DMA driver
//#include <CPU_DMA_decl.h>
//
//#include <PerformanceCounters_decl.h>
//
//// Virtual Key
//#include <VirtualKey_decl.h>
//

//// Power API
//#include <Power_decl.h>



//
// Chipset
//
/////////////////////////////////////////////////////////////////////

//#include <drivers.h>


// platform_selector.h (from MasterConfig.h)

#if defined(_WIN32)

#define GLOBAL_LOCK(x)
//#define DISABLE_INTERRUPTS()       SmartPtr_IRQ::ForceDisabled()
//#define ENABLE_INTERRUPTS()        SmartPtr_IRQ::ForceEnabled()
//#define INTERRUPTS_ENABLED_STATE() SmartPtr_IRQ::GetState()
//#define GLOBAL_LOCK_SOCKETS(x)     // UNDONE: FIXME: SmartPtr_IRQ x
#define GLOBAL_UNLOCK(x)

#if defined(_DEBUG)
#define ASSERT_IRQ_MUST_BE_OFF()   ASSERT( HAL_Windows_HasGlobalLock())
#define ASSERT_IRQ_MUST_BE_ON()    ASSERT(!HAL_Windows_HasGlobalLock())
#else
#define ASSERT_IRQ_MUST_BE_OFF()
#define ASSERT_IRQ_MUST_BE_ON()
#endif

#elif defined(__arm__)  | defined(PLATFORM_ESP32)
// nothing to define here just to help the nanoCLR VS project to build hapilly
#else
#error Unsupported platform
#endif

#endif  // _NANOHAL_H_
