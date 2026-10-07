/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "debug_log.h"
#include "m4_ipc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static uint32_t shared_value = 0U;
static uint32_t shared_op_seq = 0U;
/* USER CODE END Variables */
/* Definitions for HeartBeatTask */
osThreadId_t HeartBeatTaskHandle;
const osThreadAttr_t HeartBeatTask_attributes = {
  .name = "HeartBeatTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UARTTask */
osThreadId_t UARTTaskHandle;
const osThreadAttr_t UARTTask_attributes = {
  .name = "UARTTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SemProducerTask */
osThreadId_t SemProducerTaskHandle;
const osThreadAttr_t SemProducerTask_attributes = {
  .name = "SemProducerTask",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SemConsumerTask */
osThreadId_t SemConsumerTaskHandle;
const osThreadAttr_t SemConsumerTask_attributes = {
  .name = "SemConsumerTask",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MutexTaskA */
osThreadId_t MutexTaskAHandle;
const osThreadAttr_t MutexTaskA_attributes = {
  .name = "MutexTaskA",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MutexTaskB */
osThreadId_t MutexTaskBHandle;
const osThreadAttr_t MutexTaskB_attributes = {
  .name = "MutexTaskB",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SharedDataMutex */
osMutexId_t SharedDataMutexHandle;
const osMutexAttr_t SharedDataMutex_attributes = {
  .name = "SharedDataMutex"
};
/* Definitions for SyncSem */
osSemaphoreId_t SyncSemHandle;
const osSemaphoreAttr_t SyncSem_attributes = {
  .name = "SyncSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartHeartBeatTask(void *argument);
void StartUARTTask(void *argument);
void StartSemProducerTask(void *argument);
void StartSemConsumerTask(void *argument);
void StartMutexTaskA(void *argument);
void StartMutexTaskB(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of SharedDataMutex */
  SharedDataMutexHandle = osMutexNew(&SharedDataMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  if (DebugLog_Init(&huart2) != DEBUG_LOG_OK)
  {
      Error_Handler();
  }
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of SyncSem */
  SyncSemHandle = osSemaphoreNew(1, 0, &SyncSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of HeartBeatTask */
  HeartBeatTaskHandle = osThreadNew(StartHeartBeatTask, NULL, &HeartBeatTask_attributes);

  /* creation of UARTTask */
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

  /* creation of SemProducerTask */
  SemProducerTaskHandle = osThreadNew(StartSemProducerTask, NULL, &SemProducerTask_attributes);

  /* creation of SemConsumerTask */
  SemConsumerTaskHandle = osThreadNew(StartSemConsumerTask, NULL, &SemConsumerTask_attributes);

  /* creation of MutexTaskA */
  MutexTaskAHandle = osThreadNew(StartMutexTaskA, NULL, &MutexTaskA_attributes);

  /* creation of MutexTaskB */
  MutexTaskBHandle = osThreadNew(StartMutexTaskB, NULL, &MutexTaskB_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  if (M4Ipc_Init() != M4_IPC_OK)
  {
      Error_Handler();
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartHeartBeatTask */
/**
  * @brief  Function implementing the HeartBeatTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartHeartBeatTask */
void StartHeartBeatTask(void *argument)
{
  /* USER CODE BEGIN StartHeartBeatTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOB, USER_LED_Pin);
    osDelay(500U);
  }
  /* USER CODE END StartHeartBeatTask */
}

/* USER CODE BEGIN Header_StartUARTTask */
/**
* @brief Function implementing the UARTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUARTTask */
void StartUARTTask(void *argument)
{
  /* USER CODE BEGIN StartUARTTask */
  static uint32_t stack_sample_count = 0U;
  /* Infinite loop */
  for(;;)
  {
    LOG_INFO("UART", "USART2 TX OK");
    /*
    在 StartUARTTask 执行一段时间后开始检测剩余栈，
    跳过前几个周期，使任务先运行一段时间后再开始记录栈余量。
    */
    stack_sample_count ++;

    if(stack_sample_count >= 5U)
    {
      uint32_t heartbeat_stack_free;
      uint32_t uart_stack_free;
      uint32_t semproducer_stack_free;
      uint32_t semconsumer_stack_free;
      uint32_t mutex_a_stack_free;
      uint32_t mutex_b_stack_free;
      size_t heap_free;
      size_t heap_min_free;
      heartbeat_stack_free = osThreadGetStackSpace(HeartBeatTaskHandle);
      uart_stack_free = osThreadGetStackSpace(UARTTaskHandle);
      semproducer_stack_free = osThreadGetStackSpace(SemProducerTaskHandle);
      semconsumer_stack_free = osThreadGetStackSpace(SemConsumerTaskHandle);
      mutex_a_stack_free = osThreadGetStackSpace(MutexTaskAHandle);
      mutex_b_stack_free = osThreadGetStackSpace(MutexTaskBHandle);
      heap_free = xPortGetFreeHeapSize();
      heap_min_free = xPortGetMinimumEverFreeHeapSize();

      LOG_INFO("SYS",
         "HeartFree=%lu B UARTFree=%lu B M4ProdFree=%lu B M4ConsFree=%lu B",
         (unsigned long)heartbeat_stack_free,
         (unsigned long)uart_stack_free,
         (unsigned long)M4Ipc_GetProducerStackFree(),
         (unsigned long)M4Ipc_GetConsumerStackFree());

      LOG_INFO("SYS",
         "M4EvtFree=%lu B M4WorkFree=%lu B Heap=%lu B MinHeap=%lu B",
         (unsigned long)M4Ipc_GetEventProducerStackFree(),
         (unsigned long)M4Ipc_GetEventWorkStackFree(),
         (unsigned long)heap_free,
         (unsigned long)heap_min_free);

         LOG_INFO("SYS",
         "SemProFree=%lu B SemConFree=%lu B MutexAFree=%lu B MutexBFree=%lu B",
         (unsigned long)semproducer_stack_free,
         (unsigned long)semconsumer_stack_free,
         (unsigned long)mutex_a_stack_free,
         (unsigned long)mutex_b_stack_free);
    }
    osDelay(1000U);
  }
  /* USER CODE END StartUARTTask */
}

/* USER CODE BEGIN Header_StartSemProducerTask */
/**
* @brief Function implementing the SemProducerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSemProducerTask */
void StartSemProducerTask(void *argument)
{
  /* USER CODE BEGIN StartSemProducerTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(2000);

    if(osSemaphoreRelease(SyncSemHandle) == osOK)
    {
      LOG_INFO("SYS",
               "Producer Released");
    }
  }
  /* USER CODE END StartSemProducerTask */
}

/* USER CODE BEGIN Header_StartSemConsumerTask */
/**
* @brief Function implementing the SemConsumerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSemConsumerTask */
void StartSemConsumerTask(void *argument)
{
  /* USER CODE BEGIN StartSemConsumerTask */
  /* Infinite loop */
  for(;;)
  {
    LOG_INFO("SEM",
             "Consumer Waiting");

    if(osSemaphoreAcquire(SyncSemHandle, osWaitForever) == osOK)
    {
      LOG_INFO("SEM",
               "Consumer Acquired");
    }
  }
  /* USER CODE END StartSemConsumerTask */
}

/* USER CODE BEGIN Header_StartMutexTaskA */
/**
* @brief Function implementing the MutexTaskA thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMutexTaskA */
void StartMutexTaskA(void *argument)
{
  /* USER CODE BEGIN StartMutexTaskA */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1500);

    if(osMutexAcquire(SharedDataMutexHandle, osWaitForever) == osOK)
    {
      uint32_t before = shared_value;

      shared_value += 1U;

      uint32_t after = shared_value;
      uint32_t seq = ++shared_op_seq;
      osMutexRelease(SharedDataMutexHandle);

      LOG_INFO("M4M",
               "seq=%lu A value=%lu->%lu",
               (unsigned long)seq,
               (unsigned long)before,
               (unsigned long)after);
    }
  }
  /* USER CODE END StartMutexTaskA */
}

/* USER CODE BEGIN Header_StartMutexTaskB */
/**
* @brief Function implementing the MutexTaskB thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMutexTaskB */
void StartMutexTaskB(void *argument)
{
  /* USER CODE BEGIN StartMutexTaskB */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1500);

    if(osMutexAcquire(SharedDataMutexHandle, osWaitForever) == osOK)
    {
      uint32_t before = shared_value;

      shared_value += 10U;

      uint32_t after = shared_value;
      uint32_t seq = ++shared_op_seq;
      osMutexRelease(SharedDataMutexHandle);

      LOG_INFO("M4M",
               "seq=%lu B value=%lu->%lu",
               (unsigned long)seq,
               (unsigned long)before,
               (unsigned long)after);
    }
  }
  /* USER CODE END StartMutexTaskB */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
