#ifndef M4_ISR_H
#define M4_ISR_H

#include <stdint.h>

/* M4 ISR module status */
typedef enum
{
    M4_ISR_OK = 0,
    M4_ISR_ERROR_TIMER_START

} M4IsrStatus_t;

M4IsrStatus_t M4ISR_Init(void);

M4IsrStatus_t M4ISR_StartTim7(void);

/* Called from TIM7 interrupt context */
void M4ISR_OnTim7ElapsedFromISR(void);

#endif /* M4_ISR_H */
