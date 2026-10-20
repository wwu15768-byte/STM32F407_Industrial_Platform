#include "m4_isr.h"

#include "cmsis_os2.h"
#include "tim.h"
#include "debug_log.h"

#include <stdint.h>

#define M4_ISR_TIM7_FLAG (1UL << 0)

/* Worker Task handle */
static osThreadId_t worker_handle = NULL;

/* Runtime counters */
static volatile uint32_t irq_count = 0U;
static volatile uint32_t notify_errors = 0U;

/* Worker Task attributes */
static const osThreadAttr_t worker_attributes =
{
    .name = "M4IsrWorker",
    .stack_size = 256U * 4U,
    .priority = osPriorityNormal
};

/*
 * Start TIM7 interrupt.
 * Return explicit status instead of bool.
 */
static M4IsrStatus_t M4ISR_StartTim7(void)
{
    /* Upgrade counter */
    __HAL_TIM_SET_COUNTER(&htim7, 0U);

    /* Elimate flag of upragde event */
    __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);

    /* Start TIM7 and enable the upgrade interrupt */
    if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK)
    {
        return M4_ISR_ERROR_TIMER_START;
    }

    return M4_ISR_OK;
}

/*
 * Worker task:
 * Wait for notifications from TIM7 ISR.
 */
static void M4IsrWorkerTask(void *argument)
{
    (void)argument;

    uint32_t received_count = 0U;

    M4IsrStatus_t status = M4ISR_StartTim7();

    if (status != M4_ISR_OK)
    {
        LOG_INFO(
            "M4ISR",
            "TIM7 start FAIL, status=%lu",
            (unsigned long)status
        );

        for (;;)
        {
            osDelay(1000U);
        }
    }

    LOG_INFO("M4ISR", "TIM7 started");

    for (;;)
    {
        uint32_t flags = osThreadFlagsWait(
            M4_ISR_TIM7_FLAG,
            osFlagsWaitAny,
            osWaitForever
        );

        if ((flags & osFlagsError) != 0U)
        {
            LOG_INFO(
                "M4ISR",
                "Thread flag wait FAIL"
            );

            osDelay(100U);
            continue;
        }

        if ((flags & M4_ISR_TIM7_FLAG) != 0U)
        {
            received_count++;

            /* Print statistics every 10 notifications */
            if ((received_count % 10U) == 0U)
            {
                LOG_INFO(
                    "M4ISR",
                    "irq=%lu received=%lu errors=%lu",
                    (unsigned long)irq_count,
                    (unsigned long)received_count,
                    (unsigned long)notify_errors
                );
            }
        }
    }
}

/*
 * Initialize M4 ISR module.
 */
M4IsrStatus_t M4ISR_Init(void)
{
    if (worker_handle != NULL)
    {
        return M4_ISR_ERROR_ALREADY_INITIALIZED;
    }

    worker_handle = osThreadNew(
        M4IsrWorkerTask,
        NULL,
        &worker_attributes
    );

    if (worker_handle == NULL)
    {
        return M4_ISR_ERROR_THREAD_CREATE;
    }

    return M4_ISR_OK;
}

/*
 * Called from TIM7 ISR through HAL callback.
 * No blocking operations or UART logging.
 */
void M4ISR_OnTim7ElapsedFromISR(void)
{
    irq_count++;

    if (worker_handle == NULL)
    {
        notify_errors++;
        return;
    }

    uint32_t result = osThreadFlagsSet(
        worker_handle,
        M4_ISR_TIM7_FLAG
    );

    if ((result & osFlagsError) != 0U)
    {
        notify_errors++;
    }
}

uint32_t M4ISR_GetWorkerStackFree(void)
{
    if(worker_handle == NULL)
    {
        return 0U;
    }

    return osThreadGetStackSpace(worker_handle);
}
