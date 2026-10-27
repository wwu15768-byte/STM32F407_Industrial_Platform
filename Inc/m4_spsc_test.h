#ifndef M4_SPSC_TEST_H
#define M4_SPSC_TEST_H

#include <stdint.h>

typedef enum
{
    M4_SPSC_OK = 0,
    M4_SPSC_RUNNING,
    M4_SPSC_PASS,
    M4_SPSC_ERROR_ALREADY_INITIALIZED,
    M4_SPSC_ERROR_PRODUCER_CREATE,
    M4_SPSC_ERROR_CONSUMER_CREATE,
    M4_SPSC_ERROR_SEQUENCE,
    M4_SPSC_ERROR_TIMEOUT,
    M4_SPSC_ERROR_RING,
    M4_SPSC_ERROR_COUNTS
} M4SpscStatus_t;

/* Call from MX_FREERTOS_Init() before the scheduler starts. */
M4SpscStatus_t M4SpscTest_Init(void);
M4SpscStatus_t M4SpscTest_GetStatus(void);

/* High-water marks in bytes; 0 if the thread is not created. */
uint32_t M4SpscTest_GetProducerStackFree(void);
uint32_t M4SpscTest_GetConsumerStackFree(void);

#endif /* M4_SPSC_TEST_H */
