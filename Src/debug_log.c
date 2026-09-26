#include "debug_log.h"

#include "cmsis_os2.h"

#include <stdarg.h>
#include <stdio.h>

#define DEBUG_LOG_BUFFER_SIZE       128U /*调试输出时允许的数组大小*/
#define DEBUG_LOG_UART_TIMEOUT_MS   100U /*调试输出时允许 UART 超时的时间*/
#define DEBUG_LOG_MUTEX_TIMEOUT_TICKS  100U /*调试时允许 MUTEX 超时的时间*/

#if DEBUG_LOG_ENABLE

static UART_HandleTypeDef *s_log_uart = NULL;
static osMutexId_t s_log_mutex = NULL;
static char s_log_buffer[DEBUG_LOG_BUFFER_SIZE];

static const char *DebugLog_LevelToString(DebugLogLevel_t level)
{
    switch(level)
    {
        case DEBUG_LOG_LEVEL_DEBUG:
            return "D";

        case DEBUG_LOG_LEVEL_INFO:
            return "I";

        case DEBUG_LOG_LEVEL_WARN:
            return "W";

        case DEBUG_LOG_LEVEL_ERROR:
            return "E";

        default:
            return "?";
    }
}

DebugLogStatus_t DebugLog_Init(UART_HandleTypeDef *huart)
{
    if(huart == NULL)
    {
        return DEBUG_LOG_ERROR_INVALID_ARGUMENT;
    }

    s_log_mutex = osMutexNew(NULL); /*传入 NULL 表示默认属性*/

    if(s_log_mutex == NULL)
    {
        return DEBUG_LOG_ERROR_MUTEX;
    }

    s_log_uart = huart;

    return DEBUG_LOG_OK;
}

DebugLogStatus_t DebugLog_Write(DebugLogLevel_t level,
                                const char *tag,
                                const char *format,
                                ...)
{
    int prefix_len; /*标志字符串的长度*/
    int message_len; /*信息字符串的长度*/
    size_t used_length; /*已经输入存储数组的字符串长度*/
    /*
     * va_list 用于保存访问可变参数列表所需的状态，
     * 可以把它理解为“可变参数迭代器”。
     * 不应简单认为它一定就是指针。
     */
    va_list args;
    HAL_StatusTypeDef hal_status;

    /*防止空串口与空互斥锁*/
    if ((s_log_uart == NULL) || (s_log_mutex == NULL))
    {
        return DEBUG_LOG_ERROR_NOT_INITIALIZED;
    }

    /*防止空标签与空字符串*/
    if ((tag == NULL) || (format == NULL))
    {
        return DEBUG_LOG_ERROR_INVALID_ARGUMENT;
    }

    /*获取互斥锁并防止获取互斥锁超时*/
    if (osMutexAcquire(s_log_mutex, DEBUG_LOG_MUTEX_TIMEOUT_TICKS) != osOK)
    {
        return DEBUG_LOG_ERROR_MUTEX;
    }

    /*获取将输出内容存储到数组并获取标志字符串长度*/
    prefix_len = snprintf(s_log_buffer,
                          sizeof(s_log_buffer),
                          "[%s][%s] ",
                          DebugLog_LevelToString(level),
                          tag);

    /*输出内容做溢出保护*/
    if ((prefix_len < 0) ||
        ((size_t)prefix_len > (sizeof(s_log_buffer) - 3U)))
    {
        (void)osMutexRelease(s_log_mutex);
        return DEBUG_LOG_ERROR_FORMAT;
    }

    /*
     * va_start 用于初始化args，第二个参数必须是函数最后一个固定参数.
     * fmt 代表函数定义中最后一个固定参数，作用是帮助 `va_start()` 确定“可变参数从哪里开始”。
     */
    va_start(args, format);

    size_t message_capacity = sizeof(s_log_buffer) - (size_t)prefix_len - 3U;

    /*接收已经整理好的输出字符串长度，vsnprintf()与snprintf()核心不同是前者会在内部整理可变参数*/
    message_len = vsnprintf(&s_log_buffer[prefix_len],
                            message_capacity + 1U,
                            format,
                            args);

    va_end(args);

    if (message_len < 0)
    {
        (void)osMutexRelease(s_log_mutex);
        return DEBUG_LOG_ERROR_FORMAT;
    }

    if ((size_t)message_len > message_capacity)
    {
        message_len = (int)message_capacity;
    }

    used_length = (size_t)prefix_len + (size_t)message_len;

    /* 添加换行符，并将字符串末尾的 '\0' 后移 */
    s_log_buffer[used_length++] = '\r';
    s_log_buffer[used_length++] = '\n';
    s_log_buffer[used_length] = '\0';

    hal_status = HAL_UART_Transmit(s_log_uart,
                                   (uint8_t *)s_log_buffer,
                                   (uint16_t)used_length,
                                   DEBUG_LOG_UART_TIMEOUT_MS);

    (void)osMutexRelease(s_log_mutex);

    if (hal_status != HAL_OK)
    {
        return DEBUG_LOG_ERROR_UART;
    }

    return DEBUG_LOG_OK;
}

#else

DebugLogStatus_t DebugLog_Init(UART_HandleTypeDef *huart)
{
    (void)huart;
    return DEBUG_LOG_OK;
}

DebugLogStatus_t DebugLog_Write(DebugLogLevel_t level,
                                const char *tag,
                                const char *format,
                                ...)
{
    (void)level;
    (void)tag;
    (void)format;

    return DEBUG_LOG_OK;
}

#endif