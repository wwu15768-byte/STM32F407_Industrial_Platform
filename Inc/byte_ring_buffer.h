#ifndef BYTE_RING_BUFFER_H
#define BYTE_RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdatomic.h>
#include <stdint.h>

/*
 * Single-Producer / Single-Consumer (SPSC) byte ring buffer.
 * - Storage is caller-owned (static/global array); no dynamic allocation.
 * - Reserves one slot to distinguish full vs. empty, so usable capacity is
 *   (capacity - 1U) bytes. Minimum valid capacity is 2U.
 * - Call ByteRingBuffer_Init() only before producer/consumer start running.
 */
typedef struct
{
    uint8_t *buffer;
    uint32_t capacity;
    _Atomic uint32_t head;
    _Atomic uint32_t tail;
} ByteRingBuffer_t;

typedef enum
{
    BYTE_RB_OK = 0,
    BYTE_RB_ERROR_INVALID_ARG
} ByteRingBufferStatus_t;

ByteRingBufferStatus_t ByteRingBuffer_Init(ByteRingBuffer_t *rb,
                                           uint8_t *storage,
                                           uint32_t capacity);

/* Producer-only bulk write. Returns bytes actually stored; drops excess when full. */
uint32_t ByteRingBuffer_Write(ByteRingBuffer_t *rb,
                              const uint8_t *data,
                              uint32_t length);

/* Consumer-only bulk read. Returns bytes actually copied to data. */
uint32_t ByteRingBuffer_Read(ByteRingBuffer_t *rb,
                             uint8_t *data,
                             uint32_t max_length);

/* Approximate pending bytes (exact when called inside a critical section). */
uint32_t ByteRingBuffer_Pending(const ByteRingBuffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* BYTE_RING_BUFFER_H */
