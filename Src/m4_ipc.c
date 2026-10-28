#include "m4_ipc.h"

#include "cmsis_os2.h"
#include "debug_log.h"
#include <stdint.h>

#define M4_QUEUE_LENGTH               8U
#define M4_PRODUCER_PERIOD_MS         200U
#define M4_QUEUE_PUT_TIMEOUT_TICKS    0U

static osMessageQueueId_t s_m4_queue = NULL;

static const osMessageQueueAttr_t s_m4_queue_attributes =
{
    .name = "M4DataQueue"
};

osMessageQueueId_t M4Ipc_Init(void)
{
    s_m4_queue = osMessageQueueNew(M4_QUEUE_LENGTH,
                                   sizeof(M4QueueMessage_t),
                                   &s_m4_queue_attributes);

    osMessageQueueId_t *queue_out = s_m4_queue;
    return *queue_out;
}
