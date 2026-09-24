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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartHeartBeatTask(void *argument);
void StartUARTTask(void *argument);

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

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
  static const uint8_t usart2_test_message[] = "USART2 TX OK\r\n";
  static char txt_buff[96];
  static uint32_t stack_sample_count = 0U;
  /* Infinite loop */
  for(;;)
  {
    if( HAL_UART_Transmit(&huart2,
                          usart2_test_message,
                          (uint16_t)(sizeof(usart2_test_message) - 1U),  /*去掉 C 字符串末尾的 '\0'*/
                          100U) != HAL_OK )
    {
      Error_Handler();
    }
    /*
    在 StartUARTTask 执行一段时间后开始检测剩余栈，
    跳过前几个周期，使任务先运行一段时间后再开始记录栈余量。
    */
    stack_sample_count ++;

    if(stack_sample_count >= 5U)
    {
      uint32_t heartbeat_stack_free;
      uint32_t uart_stack_free;
      size_t heap_free;
      size_t heap_min_free;
      heartbeat_stack_free = osThreadGetStackSpace(HeartBeatTaskHandle);
      uart_stack_free = osThreadGetStackSpace(UARTTaskHandle);
      heap_free = xPortGetFreeHeapSize();
      heap_min_free = xPortGetMinimumEverFreeHeapSize();
      int len = snprintf(txt_buff,
                      sizeof(txt_buff),
                      "HeartFree = %lu B  UARTFree = %lu B Heap=%lu B MinHeap=%lu B\r\n",
                      (unsigned long)heartbeat_stack_free,
                      (unsigned long)uart_stack_free,
                      (unsigned long)heap_free,
                      (unsigned long)heap_min_free);
      if(len > 0)
      {
        uint16_t tx_len;

        if ((size_t)len >= sizeof(txt_buff))
        {
            tx_len = (uint16_t)(sizeof(txt_buff) - 1U);
        }
        else
        {
            tx_len = (uint16_t)len;
        }
        if( HAL_UART_Transmit(&huart2,
                              (uint8_t *)txt_buff,
                              tx_len,  /*去掉 C 字符串末尾的 '\0'*/
                              100U) != HAL_OK )
        {
          Error_Handler();
        }
      }
    }
    osDelay(1000U);
  }
  /* USER CODE END StartUARTTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

