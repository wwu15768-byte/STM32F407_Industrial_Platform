#include "ring_buffer.h"
#include <stddef.h>

void RingBuffer_Init(RingBuffer_t *rb)
{
    if (rb == NULL)
    {
        return;
    }

    /* No producer or consumer may run while Init executes. */
    atomic_init(&rb->head, 0U);
    atomic_init(&rb->tail, 0U);
    rb->overflow_count = 0U;
}

RingBufferStatus_t RingBuffer_WriteByte(RingBuffer_t *rb, uint32_t data)
{
    if (rb == NULL)
    {
        return RING_BUFF_ERROR_INVALID_ARG;
    }

    /* Only the producer writes head. */
    const uint32_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    const uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    const uint32_t next_head = (head + 1U) % RING_BUFFER_SIZE;

    if (next_head == tail)
    {
        rb->overflow_count++;
        return RING_BUFF_ERROR_FULL;
    }

    rb->buffer[head] = data;
    /* Publish data before making it visible to the consumer. */
    atomic_store_explicit(&rb->head, next_head, memory_order_release);
    return RING_BUFF_OK;
}

RingBufferStatus_t RingBuffer_ReadByte(RingBuffer_t *rb, uint32_t *data)
{
    if ((rb == NULL) || (data == NULL))
    {
        return RING_BUFF_ERROR_INVALID_ARG;
    }

    /* Only the consumer writes tail. */
    const uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    const uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    if (head == tail)
    {
        return RING_BUFF_ERROR_EMPTY;
    }

    *data = rb->buffer[tail];
    /* Release the consumed slot only after the read completes. */
    atomic_store_explicit(&rb->tail,
                          (tail + 1U) % RING_BUFFER_SIZE,
                          memory_order_release);
    return RING_BUFF_OK;
}

bool RingBuffer_IsEmpty(const RingBuffer_t *rb)
{
    const uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    const uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return head == tail;
}

bool RingBuffer_IsFull(const RingBuffer_t *rb)
{
    const uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    const uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return ((head + 1U) % RING_BUFFER_SIZE) == tail;
}
