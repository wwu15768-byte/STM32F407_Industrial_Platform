#include "m4_ipc.h"

#include "debug_log.h"

#define M4_QUEUE_LENGTH               8U
#define M4_PRODUCER_PERIOD_MS         200U
#define M4_QUEUE_PUT_TIMEOUT_TICKS    0U

static osMessageQueueId_t s_m4_queue = NULL;

static osThreadId_t s_m4_producer_task = NULL;
static osThreadId_t s_m4_consumer_task = NULL;

static const osMessageQueueAttr_t s_m4_queue_attributes =
{
    .name = "M4DataQueue"
};

static const osThreadAttr_t s_m4_producer_attributes =
{
    .name = "M4ProducerTask",
    .stack_size = 1024U,
    .priority = osPriorityNormal
};

static const osThreadAttr_t s_m4_consumer_attributes =
{
    .name = "M4ConsumerTask",
    .stack_size = 1024U,
    .priority = osPriorityNormal
};

static void StartM4ProducerTask(void *argument);
static void StartM4ConsumerTask(void *argument);


M4IpcStatus_t M4Ipc_Init(void)
{
    s_m4_queue = osMessageQueueNew(M4_QUEUE_LENGTH,
                                   sizeof(M4QueueMessage_t),
                                   &s_m4_queue_attributes);

    if (s_m4_queue == NULL)
    {
        return M4_IPC_ERROR_QUEUE_CREATE;
    }

    s_m4_producer_task = osThreadNew(StartM4ProducerTask,
                                     NULL,
                                     &s_m4_producer_attributes);

    if (s_m4_producer_task == NULL)
    {
        return M4_IPC_ERROR_PRODUCER_CREATE;
    }

    s_m4_consumer_task = osThreadNew(StartM4ConsumerTask,
                                     NULL,
                                     &s_m4_consumer_attributes);

    if (s_m4_consumer_task == NULL)
    {
        return M4_IPC_ERROR_CONSUMER_CREATE;
    }

    return M4_IPC_OK;
}

static void StartM4ProducerTask(void *argument)
{
    M4QueueMessage_t message = {0};
    uint32_t drop_count = 0U;

    (void)argument;

    for (;;)
    {
        message.sequence++;
        message.value = message.sequence * 10U;
        message.tick = osKernelGetTickCount();

        if (osMessageQueuePut(s_m4_queue,
                              &message,
                              0U,
                              M4_QUEUE_PUT_TIMEOUT_TICKS) != osOK)
        {
            drop_count++;

            if ((drop_count == 1U) ||
                ((drop_count % 10U) == 0U))
            {
                LOG_WARN("M4Q",
                         "queue full: drop=%lu seq=%lu count=%lu",
                         (unsigned long)drop_count,
                         (unsigned long)message.sequence,
                         (unsigned long)osMessageQueueGetCount(s_m4_queue));
            }
        }

        osDelay(M4_PRODUCER_PERIOD_MS);
    }
}

static void StartM4ConsumerTask(void *argument)
{
    M4QueueMessage_t message;
    uint32_t expected_sequence = 1U;

    (void)argument;

    for (;;)
    {
        if (osMessageQueueGet(s_m4_queue,
                              &message,
                              NULL,
                              osWaitForever) == osOK)
        {
            if (message.sequence != expected_sequence)
            {
                LOG_WARN("M4Q",
                         "sequence gap: expected=%lu actual=%lu",
                         (unsigned long)expected_sequence,
                         (unsigned long)message.sequence);
            }

            LOG_INFO("M4Q",
                     "rx seq=%lu value=%lu tick=%lu count=%lu",
                     (unsigned long)message.sequence,
                     (unsigned long)message.value,
                     (unsigned long)message.tick,
                     (unsigned long)osMessageQueueGetCount(s_m4_queue));

            expected_sequence = message.sequence + 1U;
        }
    }
}

uint32_t M4Ipc_GetProducerStackFree(void)
{
    if (s_m4_producer_task == NULL)
    {
        return 0U;
    }

    return osThreadGetStackSpace(s_m4_producer_task);
}

uint32_t M4Ipc_GetConsumerStackFree(void)
{
    if (s_m4_consumer_task == NULL)
    {
        return 0U;
    }

    return osThreadGetStackSpace(s_m4_consumer_task);
}
