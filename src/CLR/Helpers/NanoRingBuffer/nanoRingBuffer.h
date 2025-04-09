//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANO_RINGBUFFER_H_
#define _NANO_RINGBUFFER_H_

#include <nanoCLR_Headers.h>
#include <nanoHAL_v2.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {

    size_t _size;
    size_t _capacity;
    size_t _write_index;
    size_t _read_index;
    uint8_t *_buffer;

} NanoRingBuffer;

/// 
/// @brief Initialize the ring bugger.
/// 
/// @param object Pointer to the NanoRingBuffer object that contains the structure to be initialized.
/// @param buffer Pointer to the data buffer that will store the ring buffer data.
/// @param size The size of the ring buffer.
///
void NanoRingBuffer_Initialize(NanoRingBuffer *object, uint8_t *buffer, size_t size);

/// 
/// @brief Returns the capacity of the ring buffer.
/// 
/// @param object Pointer to the NanoRingBuffer object on which the operation will be performed.
/// @return size_t The capacity of the ring buffer.
///
size_t NanoRingBuffer_Capacity(NanoRingBuffer *object);

/// 
/// @brief Returns the number of bytes stored in the ring buffer.
/// 
/// @param object Pointer to the NanoRingBuffer object on which the operation will be performed.
/// @return size_t The number of bytes stored in the ring buffer.
///
size_t NanoRingBuffer_Size(NanoRingBuffer *object);

/// 
/// @brief Pushes a single element to the buffer.
/// 
/// @param object Pointer to the NanoRingBuffer object on which the operation will be performed.
/// @param data Element to be stored in the ring buffer.
/// @return size_t Number of bytes stored in the ring buffer. Will be 0 if the buffer is full. 
///
size_t NanoRingBuffer_Push(NanoRingBuffer *object, const uint8_t data);

/// 
/// @brief Pushes N elements to the buffer.
/// 
/// @param object Pointer to the NanoRingBuffer object on which the operation will be performed.
/// @param data Pointer to the buffer to be stored in the ring buffer.
/// @param length Size of the buffer to store.
/// @return size_t Number of bytes stored in the ring buffer. Will be 0 if the buffer is full. 
///
size_t NanoRingBuffer_PushN(NanoRingBuffer *object, const uint8_t *data, size_t length);

/// 
/// @brief Pop N elements from ring buffer returning them in the data argument.
/// 
/// @param object Pointer to the NanoRingBuffer object on which the operation will be performed.
/// @param data Pointer where the elemets to be poped will be copied to from the the ring buffer.
/// @param length Number of elements to try popping from the ring buffer.
/// @return size_t Number of elements actually popped from the ring buffer.
///
size_t NanoRingBuffer_PopN(NanoRingBuffer *object, uint8_t *data, size_t length);

/// 
/// @brief Pop N elements from ring buffer. The elements are not actually returned, just popped from the ring buffer.
/// 
/// @param object Pointer to the NanoRingBuffer object on which the operation will be performed.
/// @param length Number of elements to try popping from the ring buffer.
/// @return size_t Number of elements actually popped from the ring buffer.
///
size_t NanoRingBuffer_Pop(NanoRingBuffer *object, size_t length);

/// 
/// @brief Optimizes the sequence of the storage buffer so all elements are contiguous.
/// 
///
void OptimizeSequence(NanoRingBuffer *object);

#ifdef __cplusplus
}
#endif

#endif // _NANO_RINGBUFFER_H_
