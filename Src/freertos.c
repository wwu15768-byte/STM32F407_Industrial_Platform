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
#include "ring_buffer.h"
#include "m4_isr.h"
#include "adc.h"
#include "tim.h"
#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#if defined(M4_ENABLE_SPSC_TEST)
#include "m4_spsc_test.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_FLAG_HALF (1UL << 0)
#define ADC_FLAG_FULL (1UL << 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#define ADC_BUFFER_SIZE 20U

static uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint32_t voltage = 0;

volatile uint32_t adc_half_count = 0U;
volatile uint32_t adc_full_count = 0U;
volatile uint32_t adc_error_count = 0U;
volatile uint32_t adc_half_processed = 0U;
volatile uint32_t adc_full_processed = 0U;
volatile uint32_t adc_half_tick = 0U;
volatile uint32_t adc_full_tick = 0U;
volatile uint32_t adc_late_count = 0U;

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
/* Definitions for AcquisitionTask */
osThreadId_t AcquisitionTaskHandle;
const osThreadAttr_t AcquisitionTask_attributes = {
  .name = "AcquisitionTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartHeartBeatTask(void *argument);
void StartUARTTask(void *argument);
void StartAcquisitionTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  if (DebugLog_Init(&huart2) != DEBUG_LOG_OK)
  {
      Error_Handler();
  }
  /* USER CODE END RTOS_MUTEX */

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

  /* creation of AcquisitionTask */
  AcquisitionTaskHandle = osThreadNew(StartAcquisitionTask, NULL, &AcquisitionTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
#if defined(M4_ENABLE_SPSC_TEST)
  if (M4SpscTest_Init() != M4_SPSC_OK)
  {
      Error_Handler();
  }
#endif
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

    /*
    检测剩余栈与运行状态
    */
    stack_sample_count ++;

    if(stack_sample_count % 10 == 0U)
    {
      uint32_t heartbeat_stack_free;
      uint32_t uart_stack_free;

      size_t heap_free;
      size_t heap_min_free;
      heartbeat_stack_free = osThreadGetStackSpace(HeartBeatTaskHandle);
      uart_stack_free = osThreadGetStackSpace(UARTTaskHandle);

      heap_free = xPortGetFreeHeapSize();
      heap_min_free = xPortGetMinimumEverFreeHeapSize();
      LOG_INFO("UART", "USART2 TX OK");
      LOG_INFO("SYS",
         "HeartFree=%lu B UARTFree=%lu B ",
         (unsigned long)heartbeat_stack_free,
         (unsigned long)uart_stack_free);

      LOG_INFO("SYS",
         "Heap=%lu B MinHeap=%lu B",
         (unsigned long)heap_free,
         (unsigned long)heap_min_free);
      LOG_INFO("ADC",
         "Voltage=%lu mV Error=%lu",
         (unsigned long)voltage,
         (unsigned long)adc_error_count);

      LOG_INFO("DMA",
         "Half=%lu/%lu Full=%lu/%lu",
         (unsigned long)adc_half_processed,
         (unsigned long)adc_half_count,
         (unsigned long)adc_full_processed,
         (unsigned long)adc_full_count);

      LOG_INFO("DMA", "Late=%lu",
         (unsigned long)adc_late_count);

#if defined(M4_ENABLE_SPSC_TEST)
      LOG_INFO(
            "SYS",
            "SpscPFree=%lu B SpscCFree=%lu B",
            (unsigned long)M4SpscTest_GetProducerStackFree(),
            (unsigned long)M4SpscTest_GetConsumerStackFree());
#endif
    }
    osDelay(1000U);
  }
  /* USER CODE END StartUARTTask */
}

/* USER CODE BEGIN Header_StartAcquisitionTask */
/**
* @brief Function implementing the AcquisitionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAcquisitionTask */
void StartAcquisitionTask(void *argument)
{
  /* USER CODE BEGIN StartAcquisitionTask */
  uint32_t flag;
  uint32_t sum = 0U;
  uint32_t adc_average = 0U;
  if (HAL_ADC_Start_DMA(
    &hadc1,
    (uint32_t *)adc_buffer,
    ADC_BUFFER_SIZE) != HAL_OK)
  {
      Error_Handler();
  }

  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
      Error_Handler();
  }
  /* Infinite loop */
  for(;;)
  {
    flag = osThreadFlagsWait(ADC_FLAG_HALF | ADC_FLAG_FULL,
                                      osFlagsWaitAny,
                                      osWaitForever);

    if ((flag & osFlagsError) != 0U)
    {
        Error_Handler();
    }

    if ((flag & ADC_FLAG_HALF) != 0U)
    {
        if (osKernelGetTickCount() - adc_half_tick >= 200U)
        {
            adc_late_count++;
        }
        sum = 0U;

        for(int i = 0; i < 10; i++)
        {
            sum += adc_buffer[i];
        }
        adc_half_processed++;

        adc_average = sum / 10U;
        voltage = (adc_average * 3300U + 2047U) / 4095U;
    }

    if ((flag & ADC_FLAG_FULL) != 0U)
    {
        if (osKernelGetTickCount() - adc_full_tick >= 200U)
        {
            adc_late_count++;
        }
        sum = 0U;

        for(int i = 10; i < 20; i++)
        {
            sum += adc_buffer[i];
        }
        adc_full_processed++;

        adc_average = sum / 10U;
        voltage = (adc_average * 3300U + 2047U) / 4095U;
    }

  }
  /* USER CODE END StartAcquisitionTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC1)
    {
        adc_half_count++;
        adc_half_tick = osKernelGetTickCount();
        if(AcquisitionTaskHandle != NULL)
        {
            osThreadFlagsSet(AcquisitionTaskHandle,
                            ADC_FLAG_HALF);
        }
    }


}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_full_count++;
        adc_full_tick = osKernelGetTickCount();
        if(AcquisitionTaskHandle != NULL)
        {
            osThreadFlagsSet(AcquisitionTaskHandle,
                             ADC_FLAG_FULL);
        }
    }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {

        adc_error_count++;
    }
}
/* USER CODE END Application */
