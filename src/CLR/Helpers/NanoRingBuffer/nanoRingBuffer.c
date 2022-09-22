//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoRingBuffer.h"

void OptimizeSequence(NanoRingBuffer *object)
{
    // no elements, so there is nothing to optimize
    if (object->_size == 0)
    {
        return;
    }

    // read index is already at index 0, so there is nothing to optimize
    if (object->_read_index == 0)
    {
        return;
    }

    // can move data in a single memcpy
    if (object->_read_index < object->_write_index)
    {
        // buffer looks like this
        // |...xxxxx.....|
        memcpy(object->_buffer, object->_buffer + object->_read_index, object->_size);
    }
    // need to move data in two steps
    else
    {
        // buffer looks like this
        // |xxxx......xxxxxx|

        // store size of tail
        size_t tailSize = object->_write_index - sizeof(uint8_t);

        // 1st move tail to temp buffer (need to malloc first)
        uint8_t *tempBuffer = (uint8_t *)platform_malloc(tailSize);

        memcpy(tempBuffer, object->_buffer, tailSize);

        // store size of remaining buffer
        size_t headSize = object->_capacity - object->_read_index;

        // 2nd move head to start of buffer
        memcpy(object->_buffer, object->_buffer + object->_read_index, headSize);

        // 3rd move temp buffer after head
        memcpy(object->_buffer + headSize, tempBuffer, tailSize);

        // free memory
        platform_free(tempBuffer);
    }

    // adjust indexes
    object->_read_index = 0;
    object->_write_index = object->_size;
}

void NanoRingBuffer_Initialize(NanoRingBuffer *object, uint8_t *buffer, size_t size)
{
    object->_capacity = size;
    object->_write_index = 0;
    object->_read_index = 0;
    object->_size = 0;

    object->_buffer = buffer;
}

size_t NanoRingBuffer_Capacity(NanoRingBuffer *object)
{
    return object->_capacity;
}

size_t NanoRingBuffer_Size(NanoRingBuffer *object)
{
    return object->_size;
}

size_t NanoRingBuffer_Push(NanoRingBuffer *object, const uint8_t data)
{
    // check for buffer full
    if (object->_size == object->_capacity)
    {
        // buffer full
        return 0;
    }

    uint8_t *destination = object->_buffer;
    destination += object->_write_index;

    *destination = data;
    object->_write_index++;

    // check if we are the end of the capacity
    if (object->_write_index == object->_capacity)
    {
        object->_write_index = 0;
    }

    // update ring buffer size
    object->_size++;

    ASSERT(object->_write_index <= object->_capacity);
    ASSERT(object->_size <= object->_capacity);

    return 1;
}

size_t NanoRingBuffer_PushN(NanoRingBuffer *object, const uint8_t *data, size_t length)
{
    size_t lengthToWrite = 0;

    // sanity check for 0 length
    if (length == 0)
    {
        return 0;
    }

    // check for buffer full
    if (object->_size == object->_capacity)
    {
        // buffer full
        return 0;
    }

    if (length <= (object->_capacity - object->_size))
    {
        lengthToWrite = length;
    }
    else
    {
        lengthToWrite = (object->_capacity - object->_size);
    }

    uint8_t *destination = object->_buffer;
    destination += object->_write_index;

    // single memcpy
    if (lengthToWrite <= object->_capacity - object->_write_index)
    {
        memcpy(destination, data, lengthToWrite);
        object->_write_index += lengthToWrite;

        // check if we are the end of the capacity
        if (object->_write_index == object->_capacity)
        {
            object->_write_index = 0;
        }
    }
    // need to memcpy in two chunks
    else
    {
        size_t chunk1Size = object->_capacity - object->_write_index;
        memcpy(destination, data, chunk1Size);

        data += chunk1Size;

        size_t chunk2Size = lengthToWrite - chunk1Size;
        memcpy(object->_buffer, data, chunk2Size);

        object->_write_index = chunk2Size;
    }

    // update ring buffer size
    object->_size += lengthToWrite;

    ASSERT(object->_write_index <= object->_capacity);
    ASSERT(object->_size <= object->_capacity);

    return lengthToWrite;
}

size_t NanoRingBuffer_PopN(NanoRingBuffer *object, uint8_t *data, size_t length)
{
    size_t lengthToRead = 0;

    // sanity check for 0 length
    if (length == 0)
    {
        return 0;
    }

    // check for buffer empty
    if (object->_size == 0)
    {
        return 0;
    }

    if (length > object->_size)
    {
        lengthToRead = object->_size;
    }
    else
    {
        lengthToRead = length;
    }

    ASSERT(lengthToRead <= object->_size);

    uint8_t *source = object->_buffer;
    source += object->_read_index;

    // can read in a single memcpy
    if (lengthToRead <= object->_capacity - object->_read_index)
    {
        memcpy(data, source, lengthToRead);
        object->_read_index += lengthToRead;

        // check if we are at end of capacity
        if (object->_read_index == object->_capacity)
        {
            object->_read_index = 0;
        }
    }
    // need to memcpy in two steps
    else
    {
        size_t chunk1Size = object->_capacity - object->_read_index;
        memcpy(data, source, chunk1Size);

        data += chunk1Size;
        
        size_t chunk2Size = lengthToRead - chunk1Size;
        memcpy(data, object->_buffer, chunk2Size);

        object->_read_index = chunk2Size;
    }

    // update ring buffer size
    object->_size -= lengthToRead;

    // check for optimization to improve sequential push
    // buffer has to be empty and read and write indexes coincide
    if (object->_size == 0 && (object->_write_index == object->_read_index))
    {
        // reset the read/write index
        object->_write_index = 0;
        object->_read_index = 0;
    }

    ASSERT(object->_read_index <= object->_capacity);
    ASSERT(object->_size <= object->_capacity);

    return lengthToRead;
}

size_t NanoRingBuffer_Pop(NanoRingBuffer *object, size_t length)
{
    size_t lengthToRead = 0;

    // sanity check for 0 length
    if (length == 0)
    {
        return 0;
    }

    // check for buffer empty
    if (object->_size == 0)
    {
        return 0;
    }

    if (length > object->_size)
    {
        lengthToRead = object->_size;
    }
    else
    {
        lengthToRead = length;
    }

    // can read in a single memcpy
    if (lengthToRead <= object->_capacity - object->_read_index)
    {
        object->_read_index += lengthToRead;

        // check if we are at end of capacity
        if (object->_read_index == object->_capacity)
        {
            object->_read_index = 0;
        }
    }
    // need to memcpy in two steps
    else
    {
        size_t chunk1Size = object->_capacity - object->_read_index;
        size_t chunk2Size = lengthToRead - chunk1Size;
        object->_read_index = chunk2Size;
    }

    // update ring buffer size
    object->_size -= lengthToRead;

    // check for optimization to improve sequential push
    // buffer has to be empty and read and write indexes coincide
    if (object->_size == 0 && (object->_write_index == object->_read_index))
    {
        // reset the read/write index
        object->_write_index = 0;
        object->_read_index = 0;
    }

    ASSERT(object->_read_index <= object->_capacity);
    ASSERT(object->_size <= object->_capacity);

    return lengthToRead;
}
