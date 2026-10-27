#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#define RING_BUFFER_SIZE 8U

/* SPSC: 1 producer owns head/overflow_count; 1 consumer owns tail.
 * Head and tail are atomic publication indexes. Capacity is 7 uint32_t elements.
 * Call Init only before either task starts, never during transfer.
 */
typedef struct
{
    uint32_t buffer[RING_BUFFER_SIZE];
    _Atomic uint32_t head;
    _Atomic uint32_t tail;
    uint32_t overflow_count; /* Producer writes; read only when producer is done. */
} RingBuffer_t;

typedef enum
{
    RING_BUFF_OK = 0,
    RING_BUFF_ERROR_INVALID_ARG,
    RING_BUFF_ERROR_FULL,
    RING_BUFF_ERROR_EMPTY
} RingBufferStatus_t;

void RingBuffer_Init(RingBuffer_t *rb);

/* Historical API names: each element is uint32_t, NOT one byte. */
RingBufferStatus_t RingBuffer_WriteByte(RingBuffer_t *rb, uint32_t data);
RingBufferStatus_t RingBuffer_ReadByte(RingBuffer_t *rb, uint32_t *data);

/* Legacy query APIs retained to preserve prior tests. */
bool RingBuffer_IsEmpty(const RingBuffer_t *rb);
bool RingBuffer_IsFull(const RingBuffer_t *rb);

#endif /* RING_BUFFER_H */
