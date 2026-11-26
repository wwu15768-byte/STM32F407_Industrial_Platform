#include "byte_ring_buffer.h"

#include <stddef.h>

ByteRingBufferStatus_t ByteRingBuffer_Init(ByteRingBuffer_t *rb,
                                           uint8_t *storage,
                                           uint32_t capacity)
{
    if ((rb == NULL) || (storage == NULL) || (capacity < 2U))
    {
        return BYTE_RB_ERROR_INVALID_ARG;
    }

    rb->buffer = storage;
    rb->capacity = capacity;
    atomic_init(&rb->head, 0U);
    atomic_init(&rb->tail, 0U);

    return BYTE_RB_OK;
}

uint32_t ByteRingBuffer_Write(ByteRingBuffer_t *rb,
                              const uint8_t *data,
                              uint32_t length)
{
    if ((rb == NULL) || (rb->buffer == NULL) || (rb->capacity < 2U) ||
        (data == NULL) || (length == 0U))
    {
        return 0U;
    }

    uint32_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    const uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    const uint32_t capacity = rb->capacity;
    uint32_t written = 0U;

    for (; written < length; ++written)
    {
        uint32_t next = head + 1U;
        if (next >= capacity)
        {
            next = 0U;
        }

        if (next == tail)
        {
            break; /* Ring full: preserve unread older bytes. */
        }

        rb->buffer[head] = data[written];
        head = next;
    }

    /* Publish written bytes before advancing head. */
    atomic_store_explicit(&rb->head, head, memory_order_release);
    return written;
}

uint32_t ByteRingBuffer_Read(ByteRingBuffer_t *rb,
                             uint8_t *data,
                             uint32_t max_length)
{
    if ((rb == NULL) || (rb->buffer == NULL) || (rb->capacity < 2U) ||
        (data == NULL) || (max_length == 0U))
    {
        return 0U;
    }

    uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    const uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    const uint32_t capacity = rb->capacity;
    uint32_t count = 0U;

    for (; (count < max_length) && (tail != head); ++count)
    {
        data[count] = rb->buffer[tail];
        tail++;
        if (tail >= capacity)
        {
            tail = 0U;
        }
    }

    /* Release consumed slots only after copying bytes out. */
    atomic_store_explicit(&rb->tail, tail, memory_order_release);
    return count;
}

uint32_t ByteRingBuffer_Pending(const ByteRingBuffer_t *rb)
{
    if ((rb == NULL) || (rb->capacity < 2U))
    {
        return 0U;
    }

    const uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    const uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    if (head >= tail)
    {
        return head - tail;
    }

    return (rb->capacity - tail) + head;
}
