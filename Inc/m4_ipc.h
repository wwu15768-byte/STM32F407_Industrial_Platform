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
} M4IpcStatus_t;

osMessageQueueId_t M4Ipc_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* M4_IPC_H */
