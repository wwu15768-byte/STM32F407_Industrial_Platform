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
#include "cmsis_os2.h"
#include "portmacro.h"
#include "projdefs.h"
#include "stm32f407xx.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "debug_log.h"
#include "byte_ring_buffer.h"
#include "adc.h"
#include "tim.h"
#include <stdatomic.h>
#include <stdint.h>
#include <stddef.h>
#if defined(M4_ENABLE_SPSC_TEST)
#include "m4_spsc_test.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  M6_RX_RUNNING = 0U,
  M6_RX_FAULT_PENDING,
  M6_RX_RECOVERING,
  M6_RX_RETRY_WAIT,
  M6_RX_FAILED
} M6_RxState_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_FLAG_HALF           (1UL << 0)
#define ADC_FLAG_FULL           (1UL << 1)

#define ADC_BUFFER_SIZE         20U

#define M6_RX_DMA_SIZE          256U
#define M6_RING_BUFF_SIZE       1024U
#define M6_RX_MAX_RETRIES       3U
#define M6_RX_RETRY_DELAY_MS    100U
#define SYS_SUMMARY_PERIOD_SEC  5U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
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

static uint8_t m6_dma_rx_buf[M6_RX_DMA_SIZE];
static uint8_t m6_rx_ring_storage[M6_RING_BUFF_SIZE];
static ByteRingBuffer_t m6_rx_rb;

static volatile uint32_t m6_dma_old_pos = 0U;
static volatile uint32_t m6_start_fail = 0U;

static volatile uint32_t m6_observed_byte = 0U;
static volatile uint32_t m6_written_byte = 0U;
static volatile uint32_t m6_drop_byte = 0U;
static volatile uint32_t m6_consumed_byte = 0U;

static volatile uint32_t m6_last_observed_byte = 0U;
static volatile uint32_t m6_last_written_byte = 0U;
static volatile uint32_t m6_last_drop_byte = 0U;
static volatile uint32_t m6_last_consumed_byte = 0U;
static volatile uint32_t m6_last_pending_byte = 0U;
static volatile uint32_t m6_stats_error_count = 0U;

static volatile uint32_t m6_uart_error_count = 0U;
static volatile uint32_t m6_uart_pe_count = 0U;
static volatile uint32_t m6_uart_ne_count = 0U;
static volatile uint32_t m6_uart_fe_count = 0U;
static volatile uint32_t m6_uart_ore_count = 0U;
static volatile uint32_t m6_uart_dma_error_count = 0U;
static volatile uint32_t m6_uart_last_error = 0U;

static _Atomic uint32_t m6_rx_state = M6_RX_RUNNING;

static volatile uint32_t m6_rx_recover_attempts = 0U;
static volatile uint32_t m6_rx_recover_start_ok = 0U;
static volatile uint32_t m6_recover_attempt_fail = 0U;
static volatile uint32_t m6_rx_recover_exhausted = 0U;
static volatile uint32_t m6_rx_gap_count = 0U;
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
static HAL_StatusTypeDef M6_UART_RxStart(void)
{
  return HAL_UARTEx_ReceiveToIdle_DMA(&huart2,
                                      m6_dma_rx_buf,
                                      M6_RX_DMA_SIZE);
}

static void M6_GetRxStats(void)
{
  taskENTER_CRITICAL();

  m6_last_consumed_byte = m6_consumed_byte;
  m6_last_drop_byte = m6_drop_byte;
  m6_last_observed_byte = m6_observed_byte;
  m6_last_written_byte = m6_written_byte;
  m6_last_pending_byte = ByteRingBuffer_Pending(&m6_rx_rb);

  taskEXIT_CRITICAL();

  if ((m6_last_observed_byte != (m6_last_drop_byte + m6_last_written_byte)) ||
      (m6_last_written_byte != (m6_last_consumed_byte + m6_last_pending_byte)))
  {
    m6_stats_error_count++;
  }
}

static void M6_UART_RecordError(uint32_t error)
{
  m6_uart_last_error = error;
  m6_uart_error_count++;

  if ((error & HAL_UART_ERROR_PE) != 0U)
  {
    m6_uart_pe_count++;
  }

  if ((error & HAL_UART_ERROR_NE) != 0U)
  {
    m6_uart_ne_count++;
  }

  if ((error & HAL_UART_ERROR_FE) != 0U)
  {
    m6_uart_fe_count++;
  }

  if ((error & HAL_UART_ERROR_ORE) != 0U)
  {
    m6_uart_ore_count++;
  }

  if ((error & HAL_UART_ERROR_DMA) != 0U)
  {
    m6_uart_dma_error_count++;
  }
}

static HAL_StatusTypeDef M6_UART_RecoverRx(void)
{
  DMA_HandleTypeDef *hdma = huart2.hdmarx;
  HAL_StatusTypeDef status;

  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Wait while HAL DMA abort is still in progress. */
  if (HAL_DMA_GetState(hdma) == HAL_DMA_STATE_ABORT)
  {
    return HAL_BUSY;
  }

  /* UART already cleared DMAR, but DMA stream has not returned to READY yet. */
  if ((HAL_DMA_GetState(hdma) == HAL_DMA_STATE_BUSY) &&
      ((huart2.Instance->CR3 & USART_CR3_DMAR) == 0U))
  {
    return HAL_BUSY;
  }

  /* Explicitly abort RX only if UART RX or DMAR is still active. */
  if ((huart2.RxState != HAL_UART_STATE_READY) ||
      ((huart2.Instance->CR3 & USART_CR3_DMAR) != 0U))
  {
    status = HAL_UART_AbortReceive(&huart2);
    if (status != HAL_OK)
    {
      return status;
    }
  }

  /* Ensure both UART RX and DMA stream are completely idle before restart. */
  if ((huart2.RxState != HAL_UART_STATE_READY) ||
      (HAL_DMA_GetState(hdma) != HAL_DMA_STATE_READY) ||
      ((huart2.Instance->CR3 & USART_CR3_DMAR) != 0U) ||
      ((hdma->Instance->CR & DMA_SxCR_EN) != 0U))
  {
    return HAL_BUSY;
  }

  /* Preserve unread bytes in software ring buffer; reset DMA cursor only. */
  m6_dma_old_pos = 0U;

  return M6_UART_RxStart();
}
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
  if (ByteRingBuffer_Init(&m6_rx_rb, m6_rx_ring_storage, M6_RING_BUFF_SIZE) != BYTE_RB_OK)
  {
    Error_Handler();
  }

  if (M6_UART_RxStart() != HAL_OK)
  {
    m6_start_fail++;

    atomic_store_explicit(&m6_rx_state,
                          M6_RX_FAULT_PENDING,
                          memory_order_release);
  }
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
  static uint8_t m6_rx_chunk[64];
  uint32_t last_summary_tick = osKernelGetTickCount();
  uint32_t m6_rx_retry_count = 0U;
  uint32_t m6_next_retry_tick = 0U;
  uint32_t last_logged_rx_state = M6_RX_RUNNING;
  uint32_t last_logged_err_count = 0U;
  uint32_t last_logged_stats_err = 0U;

  const uint32_t m6_retry_delay_ticks = pdMS_TO_TICKS(M6_RX_RETRY_DELAY_MS);
  const uint32_t summary_period_ticks = SYS_SUMMARY_PERIOD_SEC * osKernelGetTickFreq();

  /* Infinite loop */
  for (;;)
  {
    uint32_t now = osKernelGetTickCount();
    uint32_t state = atomic_load_explicit(&m6_rx_state, memory_order_acquire);

    /* Fault reported by ISR: transition to bounded retry backoff. */
    if (state == M6_RX_FAULT_PENDING)
    {
      uint32_t expected = M6_RX_FAULT_PENDING;

      if (atomic_compare_exchange_strong(&m6_rx_state,
                                         &expected,
                                         M6_RX_RETRY_WAIT))
      {
        m6_rx_retry_count = 0U;
        m6_rx_gap_count++;
        m6_next_retry_tick = now + m6_retry_delay_ticks;
      }
    }
    /* Wait for retry backoff so UART/DMA can settle before recovery. */
    else if (state == M6_RX_RETRY_WAIT)
    {
      if ((int32_t)(now - m6_next_retry_tick) >= 0)
      {
        uint32_t expected = M6_RX_RETRY_WAIT;

        (void)atomic_compare_exchange_strong(&m6_rx_state,
                                             &expected,
                                             M6_RX_RECOVERING);
      }
    }

    /* Execute RX DMA recovery exclusively in task context. */
    if (atomic_load_explicit(&m6_rx_state, memory_order_acquire) == M6_RX_RECOVERING)
    {
      HAL_StatusTypeDef result = M6_UART_RecoverRx();
      uint32_t expected = M6_RX_RECOVERING;

      m6_rx_recover_attempts++;

      if (result == HAL_OK)
      {
        m6_rx_recover_start_ok++;

        (void)atomic_compare_exchange_strong(&m6_rx_state,
                                             &expected,
                                             M6_RX_RUNNING);
      }
      else
      {
        m6_recover_attempt_fail++;
        m6_rx_retry_count++;

        if (m6_rx_retry_count >= M6_RX_MAX_RETRIES)
        {
          if (atomic_compare_exchange_strong(&m6_rx_state,
                                             &expected,
                                             M6_RX_FAILED))
          {
            m6_rx_recover_exhausted++;
          }
        }
        else
        {
          m6_next_retry_tick = now + m6_retry_delay_ticks;

          (void)atomic_compare_exchange_strong(&m6_rx_state,
                                               &expected,
                                               M6_RX_RETRY_WAIT);
        }
      }
    }

    /* Drain software byte ring buffer (up to 4 x 64 B per loop). */
    uint32_t round_rx_bytes = 0U;
    for (uint32_t block = 0U; block < 4U; block++)
    {
      const uint32_t count = ByteRingBuffer_Read(&m6_rx_rb,
                                                 m6_rx_chunk,
                                                 sizeof(m6_rx_chunk));
      if (count == 0U)
      {
        break;
      }

      round_rx_bytes += count;
      m6_consumed_byte += count;
    }

    if (round_rx_bytes > 0U)
    {
      LOG_INFO("RX", "recv=%lu B", (unsigned long)round_rx_bytes);
    }

    /* Periodic 5-second compact summary covering SYS, M5 ADC, and M6 RX. */
    if ((uint32_t)(now - last_summary_tick) >= summary_period_ticks)
    {
      uint32_t heartbeat_stack_free = osThreadGetStackSpace(HeartBeatTaskHandle);
      uint32_t uart_stack_free = osThreadGetStackSpace(UARTTaskHandle);
      uint32_t acq_stack_free = osThreadGetStackSpace(AcquisitionTaskHandle);
      size_t heap_free = xPortGetFreeHeapSize();
      size_t heap_min_free = xPortGetMinimumEverFreeHeapSize();

      last_summary_tick = now;
      M6_GetRxStats();

      LOG_INFO("SYS",
               "StackFree Heart=%lu UART=%lu Acq=%lu B",
               (unsigned long)heartbeat_stack_free,
               (unsigned long)uart_stack_free,
               (unsigned long)acq_stack_free);

      LOG_INFO("SYS",
               "Heap=%lu B MinHeap=%lu B",
               (unsigned long)heap_free,
               (unsigned long)heap_min_free);

      LOG_INFO("ADC",
               "V=%lu mV Late=%lu Err=%lu",
               (unsigned long)voltage,
               (unsigned long)adc_late_count,
               (unsigned long)adc_error_count);

      LOG_INFO("RX",
               "Obs=%lu Con=%lu Drop=%lu State=%lu Err=%lu",
               (unsigned long)m6_last_observed_byte,
               (unsigned long)m6_last_consumed_byte,
               (unsigned long)m6_last_drop_byte,
               (unsigned long)atomic_load_explicit(&m6_rx_state, memory_order_acquire),
               (unsigned long)m6_uart_error_count);

#if defined(M4_ENABLE_SPSC_TEST)
      LOG_INFO("SYS",
               "SpscPFree=%lu B SpscCFree=%lu B",
               (unsigned long)M4SpscTest_GetProducerStackFree(),
               (unsigned long)M4SpscTest_GetConsumerStackFree());
#endif
    }

    /* Event-triggered error and recovery diagnostics. */
    {
      uint32_t cur_state = atomic_load_explicit(&m6_rx_state, memory_order_acquire);
      uint32_t cur_err = m6_uart_error_count;
      uint32_t cur_stats_err = m6_stats_error_count;

      if ((cur_state != last_logged_rx_state) ||
          (cur_err != last_logged_err_count) ||
          (cur_stats_err != last_logged_stats_err))
      {
        last_logged_rx_state = cur_state;
        last_logged_err_count = cur_err;
        last_logged_stats_err = cur_stats_err;

        LOG_WARN("M6ERR",
                 "St=%lu Err=%lu Last=0x%08lX PE=%lu NE=%lu FE=%lu ORE=%lu DMA=%lu",
                 (unsigned long)cur_state,
                 (unsigned long)cur_err,
                 (unsigned long)m6_uart_last_error,
                 (unsigned long)m6_uart_pe_count,
                 (unsigned long)m6_uart_ne_count,
                 (unsigned long)m6_uart_fe_count,
                 (unsigned long)m6_uart_ore_count,
                 (unsigned long)m6_uart_dma_error_count);

        LOG_WARN("M6ERR",
                 "Retry=%lu OK=%lu Fail=%lu Exh=%lu Gap=%lu StatErr=%lu",
                 (unsigned long)m6_rx_recover_attempts,
                 (unsigned long)m6_rx_recover_start_ok,
                 (unsigned long)m6_recover_attempt_fail,
                 (unsigned long)m6_rx_recover_exhausted,
                 (unsigned long)m6_rx_gap_count,
                 (unsigned long)cur_stats_err);
      }
    }

    osDelay(10U);
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


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance != USART2)
  {
    return;
  }

  if ((Size > 0U) && (Size <= M6_RX_DMA_SIZE))
  {
    uint32_t remaining = __HAL_DMA_GET_COUNTER(huart->hdmarx);
    uint32_t pos = (M6_RX_DMA_SIZE - remaining) % M6_RX_DMA_SIZE;

    if (pos > m6_dma_old_pos)
    {
      uint32_t len = pos - m6_dma_old_pos;
      uint32_t written = ByteRingBuffer_Write(&m6_rx_rb,
                                              &m6_dma_rx_buf[m6_dma_old_pos],
                                              len);
      m6_drop_byte += (len - written);
      m6_written_byte += written;
      m6_observed_byte += len;
      m6_dma_old_pos = pos;
    }
    else if (pos < m6_dma_old_pos)
    {
      uint32_t tail_len = M6_RX_DMA_SIZE - m6_dma_old_pos;
      uint32_t written = ByteRingBuffer_Write(&m6_rx_rb,
                                              &m6_dma_rx_buf[m6_dma_old_pos],
                                              tail_len);
      m6_drop_byte += (tail_len - written);
      m6_written_byte += written;
      m6_observed_byte += tail_len;

      if (pos > 0U)
      {
        written = ByteRingBuffer_Write(&m6_rx_rb, m6_dma_rx_buf, pos);
        m6_drop_byte += (pos - written);
        m6_written_byte += written;
        m6_observed_byte += pos;
      }

      m6_dma_old_pos = pos;
    }
  }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART2)
    {
        return;
    }

    uint32_t error = HAL_UART_GetError(huart);

    if (error == HAL_UART_ERROR_NONE)
    {
        return;
    }

    M6_UART_RecordError(error);

    /* don't commence blocking recovery in interrupt callback */
    if(atomic_load_explicit(&m6_rx_state,
                            memory_order_acquire) != M6_RX_FAILED)
    {
      atomic_store_explicit(&m6_rx_state,
                            M6_RX_FAULT_PENDING,
                            memory_order_release);
    }
}

/* USER CODE END Application */
