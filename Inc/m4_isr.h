#ifndef M4_ISR_H
#define M4_ISR_H

#include <stdint.h>

/* M4 ISR module status */
typedef enum
{
    M4_ISR_OK = 0,
    M4_ISR_ERROR_ALREADY_INITIALIZED,
    M4_ISR_ERROR_THREAD_CREATE,
    M4_ISR_ERROR_TIMER_START

} M4IsrStatus_t;

/* Create ISR worker task */
M4IsrStatus_t M4ISR_Init(void);

/* Called from TIM7 interrupt context */
void M4ISR_OnTim7ElapsedFromISR(void);

/* Check stack spare amount */
uint32_t M4ISR_GetWorkerStackFree(void);

#endif /* M4_ISR_H */
