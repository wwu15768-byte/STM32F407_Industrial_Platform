
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RING_BUFFER_SIZE      8U

typedef struct
{
    uint32_t buffer[RING_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;

    uint32_t overflow_count;
} RingBuffer_t;

typedef enum
{
    RING_BUFF_OK = 0,
    RING_BUFF_ERROR_INVALID_ARG,
    RING_BUFF_ERROR_FULL,
    RING_BUFF_ERROR_EMPTY
} RingBufferStatus_t;

/* 初始化 */
void RingBuffer_Init(RingBuffer_t *rb);

/* 写入一个字节 */
RingBufferStatus_t RingBuffer_WriteByte(
    RingBuffer_t *rb,
    uint32_t data);

/* 读取一个字节 */
RingBufferStatus_t RingBuffer_ReadByte(
    RingBuffer_t *rb,
    uint32_t *data);

/* 查询状态：要求 rb 非 NULL */
bool RingBuffer_IsEmpty(const RingBuffer_t *rb);

bool RingBuffer_IsFull(const RingBuffer_t *rb);

#endif /* RING_BUFFER_H */
