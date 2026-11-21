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
#include "m4_ipc.h"
#include "ring_buffer.h"
#include "m4_isr.h"
#include "adc.h"
#include "tim.h"
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
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
#define ADC_FLAG_HALF       (1UL << 0)
#define ADC_FLAG_FULL       (1UL << 1)

#define ADC_BUFFER_SIZE     20U

#define M6_RX_DMA_SIZE      256U
#define M6_RING_BUFF_SIZE   1024U
#define M6_RX_QUEUE_DEPTH   64U

#define M6_OVERFLOW_TEST    0U

#define M6_RX_MAX_RETRIES   3U
#define M6_RX_RETRY_DELAY_MS  100U


/*
* Test-only fault injection.
* 0: disabled
* 2: first two recovery calls return HAL_BUSY
* 3: all three recovery calls return HAL_BUSY
*/
#define M6_RX_TEST_BUSY_RETURNS 0U

/*
 * M6-P4 fault injection
 * 0: disabled (normal firmware)
 * 1: hardware ORE via RX DMA stall
 * 2: simulated DMA error callback
 * 3: simulated NE application path
 */
#define M6_TEST_FAULT_MODE 0U

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

static volatile uint32_t m6_rx_count = 0U;
static volatile uint32_t m6_rx_drop = 0U;
static volatile uint32_t m6_start_fail = 0U;
static volatile uint32_t m6_last_size = 0U;
static volatile uint32_t m6_last_event = 0U;
static volatile uint32_t last_logged_count = 0U;
static uint8_t m6_dma_rx_buf[M6_RX_DMA_SIZE];

static volatile uint8_t m6_data_ring_buff[M6_RING_BUFF_SIZE];

static volatile uint32_t m6_obersevd_byte = 0U;
static volatile uint32_t m6_writed_byte = 0U;
static volatile uint32_t m6_drop_byte = 0U;
static volatile uint32_t m6_consumed_byte = 0U;
static volatile uint32_t m6_pending_byte = 0U;

static volatile uint32_t m6_last_obersevd_byte = 0U;
static volatile uint32_t m6_last_writed_byte = 0U;
static volatile uint32_t m6_last_drop_byte = 0U;
static volatile uint32_t m6_last_consumed_byte = 0U;
static volatile uint32_t m6_last_pending_byte = 0U;

static volatile uint32_t m6_mismatch_byte = 0U;
static volatile uint32_t m6_stats_error_count = 0U;

static _Atomic uint32_t m6_ring_head = 0U;
static _Atomic uint32_t m6_ring_tail = 0U;
static volatile uint32_t m6_dma_old_pos = 0U;
static volatile uint32_t m6_ring_overflow_count = 0U;

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

/* M6-P4: RX recovery diagnostics */
static volatile uint32_t m6_diag_abort_wait = 0U;
static volatile uint32_t m6_diag_busy_wait = 0U;
static volatile uint32_t m6_diag_abort_rx_calls = 0U;
static volatile uint32_t m6_diag_prestart_count = 0U;

static volatile uint32_t m6_diag_uart_ready = 0U;
static volatile uint32_t m6_diag_dma_ready = 0U;
static volatile uint32_t m6_diag_dmar = 0U;
static volatile uint32_t m6_diag_dma_en = 0U;

#if (M6_RX_TEST_BUSY_RETURNS > 0U)
  static uint32_t m6_test_busy_left =
      M6_RX_TEST_BUSY_RETURNS;
#endif

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
  HAL_StatusTypeDef status;

  status = HAL_UARTEx_ReceiveToIdle_DMA(
    &huart2,
    m6_dma_rx_buf,
    M6_RX_DMA_SIZE);

  return status;
}

/* 仅由串行化的 RX 回调调用，返回实际缓存的字节数。 */
static uint32_t M6_RingWrite(const uint8_t *data, uint32_t length)
{
    uint32_t head =
        atomic_load_explicit(&m6_ring_head, memory_order_relaxed);
    const uint32_t tail =
        atomic_load_explicit(&m6_ring_tail, memory_order_acquire);
    uint32_t written = 0U;

    if (data == NULL)
    {
        return 0U;
    }

    for (; written < length; ++written)
    {
        const uint32_t next = (head + 1U) % M6_RING_BUFF_SIZE;

        if (next == tail)
        {
            break;  /* 环满，保留尚未消费的旧数据。 */
        }

        m6_data_ring_buff[head] = data[written];
        head = next;
    }

    /* 数据写完后，再一次性发布新的写入位置。 */
    atomic_store_explicit(&m6_ring_head, head, memory_order_release);
    return written;
}

static uint32_t M6_RingRead(uint8_t *data, uint32_t capacity)
{
  if(data == NULL)
  {
    return 0U;
  }
  uint32_t tail = atomic_load_explicit(&m6_ring_tail, memory_order_relaxed);
  const uint32_t head = atomic_load_explicit(&m6_ring_head, memory_order_acquire);
  uint32_t count = 0U;

  for(;(count < capacity) && (tail != head); ++count)
  {
    data[count] = m6_data_ring_buff[tail];
    tail = (tail + 1) % M6_RING_BUFF_SIZE;
  }
  atomic_store_explicit(&m6_ring_tail, tail, memory_order_release);

  return count;
}

void M6_GetRxStats(void)
{

  taskENTER_CRITICAL();

  const uint32_t head =
      atomic_load_explicit(&m6_ring_head, memory_order_relaxed);
  const uint32_t tail =
      atomic_load_explicit(&m6_ring_tail, memory_order_relaxed);

  m6_last_consumed_byte = m6_consumed_byte;
  m6_last_drop_byte = m6_drop_byte;
  m6_last_obersevd_byte = m6_obersevd_byte;
  m6_last_writed_byte = m6_writed_byte;
  m6_last_pending_byte = (head - tail + M6_RING_BUFF_SIZE) % M6_RING_BUFF_SIZE;

  taskEXIT_CRITICAL();

  if((m6_last_obersevd_byte != m6_last_drop_byte + m6_last_writed_byte) ||
     (m6_last_writed_byte != m6_last_consumed_byte + m6_last_pending_byte))
  {
      m6_stats_error_count++;
  }
}

static uint8_t M6_TestNextByte(uint32_t *state)
{
  if(state == NULL)
  {
    return 0;
  }

  uint32_t x = *state;

  x ^= x << 13U;
  x ^= x >> 17U;
  x ^= x << 5U;

  *state = x;

  return (uint8_t)(x & 0xFFU);
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

  if(hdma == NULL)
  {
    return HAL_ERROR;
  }

#if (M6_RX_TEST_BUSY_RETURNS > 0U)

  if (m6_test_busy_left > 0U)
  {
      m6_test_busy_left--;

      /* Simulate a temporarily unavailable DMA */
      return HAL_BUSY;
  }

#endif

  /* HAL's DMA is abording, which can't be restart */
  if(HAL_DMA_GetState(hdma) == HAL_DMA_STATE_ABORT)
  {
    m6_diag_abort_wait++;
    return HAL_BUSY;
  }

  /*
   * UART has requested DMA to abort but DMA is busy
   */
  if((HAL_DMA_GetState(hdma) == HAL_DMA_STATE_BUSY)&&
     ((huart2.Instance->CR3 & USART_CR3_DMAR) == 0U))
  {
    m6_diag_busy_wait++;
    return HAL_BUSY;
  }

  /*
   * Actively terminate the reception only when rx hasn't stopped
   */
  if((huart2.RxState != HAL_UART_STATE_READY) ||
     ((huart2.Instance->CR3 & USART_CR3_DMAR) != 0U))
  {
    m6_diag_abort_rx_calls++;
    status = HAL_UART_AbortReceive(&huart2);

    if(status != HAL_OK)
    {
      return status;
    }
  }

  /* inspect HAL status and DMA enable position in meantime */
  if((huart2.RxState != HAL_UART_STATE_READY) ||
     (HAL_DMA_GetState(hdma) != HAL_DMA_STATE_READY) ||
     ((huart2.Instance->CR3 & USART_CR3_DMAR) != 0U) ||
     (hdma->Instance->CR & DMA_SxCR_EN) != 0U)
  {
    return HAL_BUSY;
  }
  /* Snapshot immediately before restarting RX DMA */
  m6_diag_prestart_count++;

  m6_diag_uart_ready =
      (huart2.RxState == HAL_UART_STATE_READY) ? 1U : 0U;

  m6_diag_dma_ready =
      (HAL_DMA_GetState(hdma) == HAL_DMA_STATE_READY) ? 1U : 0U;

  m6_diag_dmar =
      ((huart2.Instance->CR3 & USART_CR3_DMAR) != 0U) ? 1U : 0U;

  m6_diag_dma_en =
      ((hdma->Instance->CR & DMA_SxCR_EN) != 0U) ? 1U : 0U;

  /* reverse ring_buff but restart dma position */
  m6_dma_old_pos = 0U;

  /* restart dma */
  return  M6_UART_RxStart();
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
  if(M6_UART_RxStart() != HAL_OK)
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
  static uint32_t stack_sample_count = 0U;
  static uint8_t m6_rx_chunk[64];
  uint32_t last_log_tick = osKernelGetTickCount();
  uint32_t expected_state = 0x13579BDFU;
  uint32_t checked_bytes = 0U;

  uint32_t m6_rx_retry_count = 0U;
  uint32_t m6_next_retry_tick = 0U;

#if (M6_TEST_FAULT_MODE != 0U)
  uint32_t m6_test_begin_tick = osKernelGetTickCount();
  uint8_t m6_test_done = 0U;
#endif

  const uint32_t m6_retry_delay_ticks = pdMS_TO_TICKS(M6_RX_RETRY_DELAY_MS);
/* M6 stress test code */
#if 0
  M6_UART_RecordError(
      HAL_UART_ERROR_FE |
      HAL_UART_ERROR_ORE
  );
#endif

#if M6_OVERFLOW_TEST

  while(atomic_load_explicit(&m6_ring_tail, memory_order_relaxed)
        == atomic_load_explicit(&m6_ring_head, memory_order_acquire))
  {
    if(osDelay(1U) != osOK)
    {
      Error_Handler();
    }
  }

  if(osDelay(2U * osKernelGetTickFreq()) != osOK)
  {
    Error_Handler();
  }

  M6_GetRxStats();

  LOG_INFO("OVF", "Obs=%lu Buf=%lu Drop=%lu Con=%lu Pending=%lu",
             (unsigned long)m6_last_obersevd_byte,
             (unsigned long)m6_last_writed_byte,
             (unsigned long)m6_last_drop_byte,
             (unsigned long)m6_last_consumed_byte,
             (unsigned long)m6_last_pending_byte);

#endif

  /* Infinite loop */
  for(;;)
  {
    uint32_t now = osKernelGetTickCount();


#if (M6_TEST_FAULT_MODE != 0U)

    if ((m6_test_done == 0U) &&
        ((uint32_t)(now - m6_test_begin_tick) >=
         pdMS_TO_TICKS(3000U)))
    {
        m6_test_done = 1U;

        DMA_HandleTypeDef *hdma = huart2.hdmarx;

        if ((hdma != NULL) &&
            (atomic_load_explicit(
                &m6_rx_state,
                memory_order_acquire) == M6_RX_RUNNING))
        {

#if (M6_TEST_FAULT_MODE == 3U)

            /* Software-only NE path test */
            if (HAL_UART_AbortReceive(&huart2) == HAL_OK)
            {
                M6_UART_RecordError(HAL_UART_ERROR_NE);

                atomic_store_explicit(
                    &m6_rx_state,
                    M6_RX_FAULT_PENDING,
                    memory_order_release);

                LOG_INFO("M6TEST", "NE_SOFTWARE_INJECTION");
            }

#else

            if ((HAL_DMA_GetState(hdma) ==
                 HAL_DMA_STATE_BUSY) &&
                ((huart2.Instance->CR3 &
                  USART_CR3_DMAR) != 0U))
            {
                /*
                 * Test only: stop RX DMA while keeping
                 * USART RX DMA requests enabled.
                 * Do not use in production.
                 */
                HAL_StatusTypeDef result =
                    HAL_DMA_Abort(hdma);

                if (result == HAL_OK)
                {

#if (M6_TEST_FAULT_MODE == 1U)

                    LOG_INFO("M6TEST", "ORE_ARMED");

#elif (M6_TEST_FAULT_MODE == 2U)

                    /*
                     * Simulate the HAL DMA error callback.
                     * No real DMA TE interrupt is generated.
                     */
                    if (hdma->XferErrorCallback != NULL)
                    {
                        hdma->XferErrorCallback(hdma);

                        LOG_INFO(
                            "M6TEST",
                            "DMA_CALLBACK_SIMULATED");
                    }

#endif
                }
                else
                {
                    LOG_INFO(
                        "M6TEST",
                        "DMA_ABORT_FAILED=%lu",
                        (unsigned long)result);
                }
            }

#endif
        }
    }

#endif


    uint32_t state = atomic_load_explicit(
        &m6_rx_state,
        memory_order_acquire);

    /* 收到新的故障，进入重试等待 */
    if (state == M6_RX_FAULT_PENDING)
    {
        uint32_t expected = M6_RX_FAULT_PENDING;

        /* CAS 保证只有一个任务能成功更新状态 */
        if (atomic_compare_exchange_strong(
                &m6_rx_state,
                &expected,
                M6_RX_RETRY_WAIT))
        {
            m6_rx_retry_count = 0U;
            m6_rx_gap_count++;

            m6_next_retry_tick = now + m6_retry_delay_ticks;
        }
    }

    /* else if 作用在于留出时间让让uart和dma就绪，到达重试时间，转入恢复状态 */
    else if (state == M6_RX_RETRY_WAIT)
    {
        if ((int32_t)(now - m6_next_retry_tick) >= 0)
        {
            uint32_t expected = M6_RX_RETRY_WAIT;

            (void)atomic_compare_exchange_strong(
                &m6_rx_state,
                &expected,
                M6_RX_RECOVERING);
        }
    }

    /* 仅由 UARTTask 执行接收恢复 */
    if (atomic_load_explicit(
            &m6_rx_state,
            memory_order_acquire) == M6_RX_RECOVERING)
    {
        HAL_StatusTypeDef result = M6_UART_RecoverRx();

        m6_rx_recover_attempts++;

        uint32_t expected = M6_RX_RECOVERING;

        if (result == HAL_OK)
        {
            m6_rx_recover_start_ok++;

            (void)atomic_compare_exchange_strong(
                &m6_rx_state,
                &expected,
                M6_RX_RUNNING);
        }
        else
        {
            m6_recover_attempt_fail++;
            m6_rx_retry_count++;

            if (m6_rx_retry_count >= M6_RX_MAX_RETRIES)
            {
                if (atomic_compare_exchange_strong(
                        &m6_rx_state,
                        &expected,
                        M6_RX_FAILED))
                {
                    m6_rx_recover_exhausted++;
                }
            }
            else
            {
                m6_next_retry_tick = now + m6_retry_delay_ticks;

                (void)atomic_compare_exchange_strong(
                    &m6_rx_state,
                    &expected,
                    M6_RX_RETRY_WAIT);
            }
        }
    }

    for(uint32_t block = 0U; block < 4U; block++)
    {
      const uint32_t count = M6_RingRead(m6_rx_chunk, sizeof(m6_rx_chunk));

      if(count == 0U)
      {
        break;
      }

      for(uint32_t i = 0U; i < count; ++i)
      {
        const uint8_t expected = M6_TestNextByte(&expected_state);

        if(m6_rx_chunk[i] != expected)
        {
          m6_mismatch_byte++;
        }

      }
      checked_bytes += count;

      m6_consumed_byte += count;

    }

    if((uint32_t)(now - last_log_tick) >= osKernelGetTickFreq())
    {
      M6_GetRxStats();
      last_log_tick = now;
      LOG_INFO( "DAR",
                "ObseredByte = %lu B WritedByte = %lu B DropByte = %lu",
                 (unsigned long)m6_last_obersevd_byte,
                 (unsigned long)m6_last_writed_byte,
                 (unsigned long)m6_last_drop_byte);

      LOG_INFO( "DAR",
                "ConsumedByte = %lu B PendingByte = %lu B",
                 (unsigned long)m6_last_consumed_byte,
                 (unsigned long)m6_last_pending_byte);

      LOG_INFO( "DAR",
                "StatsErrors = %lu",
                 (unsigned long)m6_stats_error_count);

      LOG_INFO("DAR", "CheckedBytes=%lu MismatchBytes=%lu",
                (unsigned long)checked_bytes,
                (unsigned long)m6_mismatch_byte);

      LOG_INFO(
          "M6RX",
          "State=%lu Err=%lu LastErr=0x%08lX",
          (unsigned long)atomic_load(&m6_rx_state),
          (unsigned long)m6_uart_error_count,
          (unsigned long)m6_uart_last_error);

      LOG_INFO(
          "M6RX",
          "Retry=%lu StartOK=%lu AttemptFail=%lu Exhausted=%lu Gap=%lu",
          (unsigned long)m6_rx_recover_attempts,
          (unsigned long)m6_rx_recover_start_ok,
          (unsigned long)m6_recover_attempt_fail,
          (unsigned long)m6_rx_recover_exhausted,
          (unsigned long)m6_rx_gap_count);

      LOG_INFO(
          "M6DIAG",
          "AbortWait=%lu BusyWait=%lu AbortRx=%lu PreStart=%lu",
          (unsigned long)m6_diag_abort_wait,
          (unsigned long)m6_diag_busy_wait,
          (unsigned long)m6_diag_abort_rx_calls,
          (unsigned long)m6_diag_prestart_count);

      LOG_INFO(
          "M6DIAG",
          "UReady=%lu DReady=%lu DMAR=%lu DMA_EN=%lu",
          (unsigned long)m6_diag_uart_ready,
          (unsigned long)m6_diag_dma_ready,
          (unsigned long)m6_diag_dmar,
          (unsigned long)m6_diag_dma_en);

      LOG_INFO(
          "M6ERR",
          "PE=%lu NE=%lu FE=%lu ORE=%lu DMA=%lu",
          (unsigned long)m6_uart_pe_count,
          (unsigned long)m6_uart_ne_count,
          (unsigned long)m6_uart_fe_count,
          (unsigned long)m6_uart_ore_count,
          (unsigned long)m6_uart_dma_error_count);

    }
    /*
    检测剩余栈与运行状态
    */
    stack_sample_count ++;

    if(stack_sample_count % 250 == 0U)
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

#if defined(M4_ENABLE_SPSC_TEST)
      LOG_INFO(
            "SYS",
            "SpscPFree=%lu B SpscCFree=%lu B",
            (unsigned long)M4SpscTest_GetProducerStackFree(),
            (unsigned long)M4SpscTest_GetConsumerStackFree());
#endif
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
  if(huart->Instance != USART2)
  {
    return;
  }

  if((Size > 0U) && (Size <= M6_RX_DMA_SIZE))
  {

    uint32_t remaining = __HAL_DMA_GET_COUNTER(huart->hdmarx); //获取缓存区剩余空闲位数
    uint32_t pos = (M6_RX_DMA_SIZE - remaining) % M6_RX_DMA_SIZE;

    if(pos > m6_dma_old_pos)
    {
      uint32_t written = M6_RingWrite(&m6_dma_rx_buf[m6_dma_old_pos], pos - m6_dma_old_pos);
      m6_drop_byte += pos - m6_dma_old_pos - written;
      m6_writed_byte += written;
      m6_obersevd_byte += pos - m6_dma_old_pos;
      m6_dma_old_pos = pos;
    }
    if(pos < m6_dma_old_pos)
    {
      uint32_t written = M6_RingWrite(&m6_dma_rx_buf[m6_dma_old_pos], M6_RX_DMA_SIZE - m6_dma_old_pos);
      m6_drop_byte += M6_RX_DMA_SIZE - m6_dma_old_pos - written;
      m6_writed_byte += written;
      m6_obersevd_byte += M6_RX_DMA_SIZE - m6_dma_old_pos;

      written = M6_RingWrite(m6_dma_rx_buf,  pos);
      m6_drop_byte += pos - written;
      m6_writed_byte += written;
      m6_obersevd_byte += pos;

      m6_dma_old_pos = pos;
    }
  }

  return;
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
