#include "m4_isr.h"

#include "cmsis_os2.h"
#include "tim.h"
#include "debug_log.h"

#include <stdint.h>

/*
 * Start TIM7 interrupt.
 * Return explicit status instead of bool.
 */
M4IsrStatus_t M4ISR_StartTim7(void)
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
 * Initialize M4 ISR module.
 */
M4IsrStatus_t M4ISR_Init(void)
{

    M4IsrStatus_t status = M4ISR_StartTim7();

    if(status != M4_ISR_OK)
    {
       return M4_ISR_ERROR_TIMER_START;
    }

    return M4_ISR_OK;
}

/*
 * Called from TIM7 ISR through HAL callback.
 * No blocking operations or UART logging.
 */
void M4ISR_OnTim7ElapsedFromISR(void)
{

}
