#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

/*
 *   让同一个 C 头文件既能被 C 编译器使用，也能被 C++ 编译器正确调用，
 * 使用 extern "C" 会告知 C++ 编译器将这些函数请按照 C 的链接规则处理，不要进行 C++ 名称修饰。
 *   __cplusplus 是 C++ 编译器预定义的宏，在被 C++ 编译时 __cplusplus 会被定义，条件生效，
 * 在 C++ 编译时 __cplusplus 被定义；在普通 C 编译时通常未定义，所以这部分会被跳过，不会报错。
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define DEBUG_LOG_ENABLE  1 /*日志开关，1 为打开，0 为关闭*/

#define DEBUG_LOG_LEVEL_VALUE_DEBUG  0
#define DEBUG_LOG_LEVEL_VALUE_INFO   1
#define DEBUG_LOG_LEVEL_VALUE_WARN   2
#define DEBUG_LOG_LEVEL_VALUE_ERROR  3

/*表示当前整个工程设置的“日志过滤等级/最低严重程度”。*/
#define DEBUG_LOG_LEVEL  DEBUG_LOG_LEVEL_VALUE_INFO

typedef enum
{
    DEBUG_LOG_LEVEL_DEBUG = DEBUG_LOG_LEVEL_VALUE_DEBUG,
    DEBUG_LOG_LEVEL_INFO  = DEBUG_LOG_LEVEL_VALUE_INFO,
    DEBUG_LOG_LEVEL_WARN  = DEBUG_LOG_LEVEL_VALUE_WARN,
    DEBUG_LOG_LEVEL_ERROR = DEBUG_LOG_LEVEL_VALUE_ERROR
} DebugLogLevel_t;

typedef enum
{
    DEBUG_LOG_OK = 0,
    DEBUG_LOG_ERROR_INVALID_ARGUMENT,
    DEBUG_LOG_ERROR_NOT_INITIALIZED,
    DEBUG_LOG_ERROR_MUTEX,
    DEBUG_LOG_ERROR_FORMAT,
    DEBUG_LOG_ERROR_UART
} DebugLogStatus_t;

/*
 * 把日志模块和某个 UART 绑定起来，完成初始化。
 */
DebugLogStatus_t DebugLog_Init(UART_HandleTypeDef *huart);

/*
 * 仿照 printf() 通过 UART 输出带“日志等级 + 标签 + 格式化内容”的调试信息，用法几乎相同。
 * 以后可以输出类似：
 * [DEBUG][ADC] raw=1832
 * [INFO ][SYS] System initialized
 * 的字符串
 */
DebugLogStatus_t DebugLog_Write(DebugLogLevel_t level,
                                const char *tag,
                                const char *format,
                                ...);

#if DEBUG_LOG_ENABLE

/*
 *   宏执行逻辑：进入do执行一次{...}，之后进入条件函数 while (0) 发现为假，直接退出。
 * do{...}while(0) 可以作为语句包装壳，防止在 if 中直接调用宏却没有外部大括号{}导致报错。
 * 以下宏函数的作用是过滤等级，即 INFO 及以上的等级都打印。
 * if 放在do{...}外以防编译器优化 enum 后无法为后续判断提供参数，使用 enum 外部宏更保险
 */
#if DEBUG_LOG_LEVEL <= DEBUG_LOG_LEVEL_VALUE_DEBUG

#define LOG_DEBUG(tag, format, ...)                                     \
    do                                                                  \
    {                                                                   \
        (void)DebugLog_Write(DEBUG_LOG_LEVEL_DEBUG,               \
                             tag,                                       \
                             format, ##__VA_ARGS__);                    \
    } while (0)

#else

#define LOG_DEBUG(tag, format, ...)  ((void)0)

#endif

#if DEBUG_LOG_LEVEL <= DEBUG_LOG_LEVEL_VALUE_INFO

#define LOG_INFO(tag, format, ...)                                      \
    do                                                                  \
    {                                                                   \
        (void)DebugLog_Write(DEBUG_LOG_LEVEL_INFO,                \
                             tag,                                       \
                             format, ##__VA_ARGS__);                    \
    } while (0)

#else

#define LOG_INFO(tag, format, ...)  ((void)0)

#endif

#if DEBUG_LOG_LEVEL <= DEBUG_LOG_LEVEL_VALUE_WARN

#define LOG_WARN(tag, format, ...)                                      \
    do                                                                  \
    {                                                                   \
        (void)DebugLog_Write(DEBUG_LOG_LEVEL_WARN,                \
                             tag,                                       \
                             format, ##__VA_ARGS__);                    \
    } while (0)

#else

#define LOG_WARN(tag, format, ...)   ((void)0)

#endif

#if DEBUG_LOG_LEVEL <= DEBUG_LOG_LEVEL_VALUE_ERROR

#define LOG_ERROR(tag, format, ...)                                     \
    do                                                                  \
    {                                                                   \
        (void)DebugLog_Write(DEBUG_LOG_LEVEL_ERROR,               \
                             tag,                                       \
                             format, ##__VA_ARGS__);                    \
    } while (0)

#else

#define LOG_ERROR(tag, format, ...)  ((void)0)

#endif

#else

#define LOG_DEBUG(tag, format, ...)  ((void)0)
#define LOG_INFO(tag, format, ...)   ((void)0)
#define LOG_WARN(tag, format, ...)   ((void)0)
#define LOG_ERROR(tag, format, ...)  ((void)0)

#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_LOG_H */
