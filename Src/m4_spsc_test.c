#include "m4_spsc_test.h"

#include "ring_buffer.h"
#include "debug_log.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#define SPSC_TOTAL_VALUES       50000U
#define SPSC_STACK_WORDS       256U     /* 1024 bytes on STM32F407 */
#define SPSC_STALL_TIMEOUT_MS  5000U
#define SPSC_FIRST_READ_DELAY  20U      /* Ensure the ring becomes full */

static RingBuffer_t s_rb;
static StaticTask_t s_producer_tcb;
static StaticTask_t s_consumer_tcb;
static StackType_t s_producer_stack[SPSC_STACK_WORDS];
static StackType_t s_consumer_stack[SPSC_STACK_WORDS];
static osThreadId_t s_producer = NULL;
static osThreadId_t s_consumer = NULL;

/* Shared flags are atomic; counters are owned by one task only. */
static _Atomic uint32_t s_state = M4_SPSC_OK;
static _Atomic uint32_t s_producer_done = 0U;
static _Atomic uint32_t s_written = 0U; /* Atomic for safe error snapshots. */
static uint32_t s_read = 0U;     /* Consumer owns */
static _Atomic uint32_t s_full_retries = 0U; /* Atomic for safe error snapshots. */
static uint32_t s_empty_retries = 0U; /* Consumer owns */

static void SpscProducerTask(void *argument);
static void SpscConsumerTask(void *argument);

static const osThreadAttr_t s_producer_attr =
{
    .name = "SpscProducer",
    .priority = osPriorityNormal,
    .cb_mem = &s_producer_tcb,
    .cb_size = sizeof(s_producer_tcb),
    .stack_mem = s_producer_stack,
    .stack_size = sizeof(s_producer_stack)
};

static const osThreadAttr_t s_consumer_attr =
{
    .name = "SpscConsumer",
    .priority = osPriorityNormal,
    .cb_mem = &s_consumer_tcb,
    .cb_size = sizeof(s_consumer_tcb),
    .stack_mem = s_consumer_stack,
    .stack_size = sizeof(s_consumer_stack)
};

static void SpscFail(M4SpscStatus_t failure)
{
    uint32_t expected = M4_SPSC_RUNNING;
    (void)atomic_compare_exchange_strong_explicit(
        &s_state, &expected, (uint32_t)failure,
        memory_order_acq_rel, memory_order_acquire);
}

M4SpscStatus_t M4SpscTest_Init(void)
{
    if ((s_producer != NULL) || (s_consumer != NULL))
    {
        return M4_SPSC_ERROR_ALREADY_INITIALIZED;
    }

    RingBuffer_Init(&s_rb);
    s_written = 0U;
    s_read = 0U;
    s_full_retries = 0U;
    s_empty_retries = 0U;
    atomic_store_explicit(&s_producer_done, 0U, memory_order_relaxed);
    atomic_store_explicit(&s_state, M4_SPSC_RUNNING, memory_order_release);

    s_consumer = osThreadNew(SpscConsumerTask, NULL, &s_consumer_attr);
    if (s_consumer == NULL)
    {
        atomic_store_explicit(&s_state,
                              M4_SPSC_ERROR_CONSUMER_CREATE,
                              memory_order_release);
        return M4_SPSC_ERROR_CONSUMER_CREATE;
    }

    s_producer = osThreadNew(SpscProducerTask, NULL, &s_producer_attr);
    if (s_producer == NULL)
    {
        atomic_store_explicit(&s_state,
                              M4_SPSC_ERROR_PRODUCER_CREATE,
                              memory_order_release);
        return M4_SPSC_ERROR_PRODUCER_CREATE;
    }

    return M4_SPSC_OK;
}

M4SpscStatus_t M4SpscTest_GetStatus(void)
{
    return (M4SpscStatus_t)atomic_load_explicit(&s_state,
                                                 memory_order_acquire);
}

uint32_t M4SpscTest_GetProducerStackFree(void)
{
    return (s_producer == NULL) ? 0U : osThreadGetStackSpace(s_producer);
}

uint32_t M4SpscTest_GetConsumerStackFree(void)
{
    return (s_consumer == NULL) ? 0U : osThreadGetStackSpace(s_consumer);
}

static void SpscProducerTask(void *argument)
{
    (void)argument;

    uint32_t value = 1U;
    uint32_t last_progress = osKernelGetTickCount();
    const uint32_t timeout_ticks = pdMS_TO_TICKS(SPSC_STALL_TIMEOUT_MS);

    for (; value <= SPSC_TOTAL_VALUES; )
    {
        if (M4SpscTest_GetStatus() != M4_SPSC_RUNNING)
        {
            break;
        }

        RingBufferStatus_t status = RingBuffer_WriteByte(&s_rb, value);
        if (status == RING_BUFF_OK)
        {
            atomic_fetch_add_explicit(&s_written, 1U, memory_order_relaxed);
            value++;
            last_progress = osKernelGetTickCount();
            /* Periodically let the consumer drain the buffer to exercise EMPTY. */
            if ((value % 256U) == 0U)
            {
                osDelay(2U);
            }
        }
        else if (status == RING_BUFF_ERROR_FULL)
        {
            atomic_fetch_add_explicit(&s_full_retries, 1U, memory_order_relaxed);
            if ((uint32_t)(osKernelGetTickCount() - last_progress) >= timeout_ticks)
            {
                SpscFail(M4_SPSC_ERROR_TIMEOUT);
                break;
            }
            osDelay(1U);
        }
        else
        {
            SpscFail(M4_SPSC_ERROR_RING);
            break;
        }
    }

    /* Publish producer-owned counters and its final state. */
    atomic_store_explicit(&s_producer_done, 1U, memory_order_release);

    for (;;)
    {
        osDelay(1000U);
    }
}

static void SpscConsumerTask(void *argument)
{
    (void)argument;

    uint32_t expected = 1U;
    uint32_t observed = 0U;
    uint32_t last_progress = osKernelGetTickCount();
    const uint32_t start_tick = last_progress;
    const uint32_t timeout_ticks = pdMS_TO_TICKS(SPSC_STALL_TIMEOUT_MS);

    LOG_INFO("SPSC", "START total=%lu capacity=7 elements",
             (unsigned long)SPSC_TOTAL_VALUES);

    /* The consumer is intentionally late, provoking FULL without a per-item handshake. */
    osDelay(SPSC_FIRST_READ_DELAY);

    while (expected <= SPSC_TOTAL_VALUES)
    {
        if (M4SpscTest_GetStatus() != M4_SPSC_RUNNING)
        {
            break;
        }

        RingBufferStatus_t result = RingBuffer_ReadByte(&s_rb, &observed);
        if (result == RING_BUFF_OK)
        {
            s_read++;
            if (observed != expected)
            {
                SpscFail(M4_SPSC_ERROR_SEQUENCE);
                break;
            }
            expected++;
            last_progress = osKernelGetTickCount();
        }
        else if (result == RING_BUFF_ERROR_EMPTY)
        {
            s_empty_retries++;
            if ((uint32_t)(osKernelGetTickCount() - last_progress) >= timeout_ticks)
            {
                SpscFail(M4_SPSC_ERROR_TIMEOUT);
                break;
            }
            /* Consumer does not wait for Producer's signal. */
            osDelay(1U);
        }
        else
        {
            SpscFail(M4_SPSC_ERROR_RING);
            break;
        }
    }

    /* A final consumed value may precede the producer's done flag. */
    while ((expected > SPSC_TOTAL_VALUES) &&
           (atomic_load_explicit(&s_producer_done, memory_order_acquire) == 0U) &&
           (M4SpscTest_GetStatus() == M4_SPSC_RUNNING))
    {
        if ((uint32_t)(osKernelGetTickCount() - last_progress) >= timeout_ticks)
        {
            SpscFail(M4_SPSC_ERROR_TIMEOUT);
            break;
        }
        osDelay(1U);
    }

    if (M4SpscTest_GetStatus() == M4_SPSC_RUNNING)
    {
        uint32_t sentinel = 0xA5A5A5A5U;
        RingBufferStatus_t final_read = RingBuffer_ReadByte(&s_rb, &sentinel);
        if ((s_written != SPSC_TOTAL_VALUES) ||
            (s_read != SPSC_TOTAL_VALUES) ||
            (s_full_retries == 0U) ||
            (final_read != RING_BUFF_ERROR_EMPTY) ||
            (sentinel != 0xA5A5A5A5U) ||
            (s_rb.overflow_count != s_full_retries))
        {
            SpscFail(M4_SPSC_ERROR_COUNTS);
        }
        else
        {
            atomic_store_explicit(&s_state, M4_SPSC_PASS, memory_order_release);
        }
    }

    M4SpscStatus_t verdict = M4SpscTest_GetStatus();
    if (verdict == M4_SPSC_PASS)
    {
        LOG_INFO("SPSC", "PASS sent=%lu recv=%lu full=%lu empty=%lu",
                 (unsigned long)s_written,
                 (unsigned long)s_read,
                 (unsigned long)s_full_retries,
                 (unsigned long)s_empty_retries);
        LOG_INFO("SPSC", "overflow=%lu elapsedTicks=%lu",
                 (unsigned long)s_rb.overflow_count,
                 (unsigned long)(osKernelGetTickCount() - start_tick));
    }
    else
    {
        LOG_INFO("SPSC", "FAIL code=%lu expected=%lu actual=%lu",
                 (unsigned long)verdict,
                 (unsigned long)expected,
                 (unsigned long)observed);
        LOG_INFO("SPSC", "sent=%lu recv=%lu full=%lu empty=%lu",
                 (unsigned long)s_written,
                 (unsigned long)s_read,
                 (unsigned long)s_full_retries,
                 (unsigned long)s_empty_retries);
    }

    for (;;)
    {
        osDelay(1000U);
    }
}
