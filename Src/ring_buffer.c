
#include "ring_buffer.h"
#include <stddef.h>

void RingBuffer_Init(RingBuffer_t *rb)
{
    if (rb == NULL)
    {
        return;
    }

    rb->head = 0U;
    rb->tail = 0U;

    rb->overflow_count = 0U;
}

RingBufferStatus_t RingBuffer_WriteByte(
    RingBuffer_t *rb,
    uint32_t data)
{
    if (rb == NULL)
    {
        return RING_BUFF_ERROR_INVALID_ARG;
    }

    uint32_t next_head =
        (rb->head + 1U) % RING_BUFFER_SIZE;

    if (next_head == rb->tail)
    {
        rb->overflow_count++;
        return RING_BUFF_ERROR_FULL;
    }

    rb->buffer[rb->head] = data;
    rb->head = next_head;

    return RING_BUFF_OK;
}

RingBufferStatus_t RingBuffer_ReadByte(
    RingBuffer_t *rb,
    uint32_t *data)
{
    if (rb == NULL || data == NULL)
    {
        return RING_BUFF_ERROR_INVALID_ARG;
    }

    if (rb->head == rb->tail)
    {
        return RING_BUFF_ERROR_EMPTY;
    }

    *data = rb->buffer[rb->tail];

    rb->tail =
        (rb->tail + 1U) % RING_BUFFER_SIZE;

    return RING_BUFF_OK;
}

bool RingBuffer_IsEmpty(const RingBuffer_t *rb)
{
    return rb->head == rb->tail;
}

bool RingBuffer_IsFull(const RingBuffer_t *rb)
{
    return ((rb->head + 1U) % RING_BUFFER_SIZE) == rb->tail;
}
