#ifndef M4_IPC_H
#define M4_IPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include <stdint.h>

typedef struct
{
    uint32_t sequence;
    uint32_t value;
    uint32_t tick;
} M4QueueMessage_t;

typedef enum
{
    M4_IPC_OK = 0,
    M4_IPC_ERROR_QUEUE_CREATE,
    M4_IPC_ERROR_PRODUCER_CREATE,
    M4_IPC_ERROR_CONSUMER_CREATE,
    M4_IPC_ERROR_EVENT_WORK_CREATE,
    M4_IPC_ERROR_EVENT_PRODUCER_CREATE
} M4IpcStatus_t;

M4IpcStatus_t M4Ipc_Init(void);
uint32_t M4Ipc_GetProducerStackFree(void);
uint32_t M4Ipc_GetConsumerStackFree(void);
uint32_t M4Ipc_GetEventProducerStackFree(void);
uint32_t M4Ipc_GetEventWorkStackFree(void);

#ifdef __cplusplus
}
#endif

#endif /* M4_IPC_H */
