# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-10-23

## Current Milestone

M4：RTOS 通信、同步和数据缓冲（当前阶段，尚未最终关闭）。M0～M3 已完成。

- M4.1 Queue：完成。
- M4.2 CMSIS-RTOS2 Thread Flags Task → Task：完成，已验证重复 Flag 合并。
- M4.3 Binary Semaphore / Mutex：基础验证完成；2026-10-18 前后已恢复 Semaphore 可运行基线并完成回归。
- M4.4 Ring Buffer：Empty、Full、Wrap-around、FIFO 与溢出计数确定性功能验证完成；并发安全仍未验收。
- M4.5 TIM7 ISR → Thread Flags → Worker Task：构建、首次板级与用户确认的持续运行验证通过；功能 checkpoint 已提交并推送。
- M4 Final Integration：待开始，包括模块职责收敛、联合稳定性及重启验收。

M5 ADC + Timer + DMA 尚未开始。

## Current Goal

完成 M4.5 文档与测试证据同步，并推进 M4 Final Integration。已完成的 M4.5 最小链路为：

```text
TIM7 100 ms Update IRQ
  -> TIM7_IRQHandler()
  -> HAL_TIM_PeriodElapsedCallback() / TIM7 分支
  -> M4ISR_OnTim7ElapsedFromISR() / osThreadFlagsSet()
  -> M4IsrWorkerTask / osThreadFlagsWait(osWaitForever)
```

TIM6 继续独立作为 HAL 1 ms Timebase；回调中的 `HAL_IncTick()` 只由 TIM6 分支调用。TIM7 NVIC 抢占优先级为 5，FreeRTOS 最大可调用系统 API 的数值优先级阈值为 5（4 bits 抢占优先级）。

当前不应把低负载下 `irq == received` 当作高负载无损计数保证；同一个 Thread Flag bit 允许合并。ISR 内不调用同步 `LOG_xxx()` 或阻塞接口，正式的采样数据所有权留待后续专项设计。

M4 最终关闭前还须按最新 `ring_buffer.h` 核对 Ring Buffer 数据元素类型、字节容量与历史文档记载不一致的问题；当前仍不宣称 Ring Buffer ISR/Task 并发安全。

## Completed

> 以下为按时间积累的历史完成事项，其中早期出现的“当前”只表示当时的基线；最新状态以本文件上方的 Current Milestone / 下方的 Last Verified Function 为准。

- 确认仓库根目录为 `STM32F407_Industrial_Platform`。
- 验证 STM32CubeMX、STM32CubeCLT、CMake、Ninja、GNU Arm Toolchain、CubeProgrammer CLI、ST-LINK GDB Server 和 Git。
- 初始化 Git 仓库，当前分支为 `main`。
- 配置 Git 全局提交者名称、邮箱和默认初始分支。
- 建立并验证 `.gitignore`。
- 完成 README 内容评审。
- 建立并验收 docs 项目管理体系。
- 本次提交内容已通过暂存区文件清单与空白错误检查。
- 创建第一次稳定提交 `8bc8d98`。
- 建立 GitHub 私有远程仓库并配置 `origin`。
- 验证本地 `main`、本地 `origin/main` 与 GitHub 远程 `main` 指向同一提交。
- 确认保留首次提交的旧邮箱，后续提交使用新的全局 Git 邮箱。
- 核对开发板原理图、引脚分配图、厂商资料和 MCU 数据手册。
- 确认目标为立创·天空星 STM32F407VGT6 高配版，MCU 为 LQFP-100 封装。
- 确认 HSE 8 MHz、LSE 32.768 kHz、PB2 高电平点亮 LED。
- 确认 USART2 使用 PD5/PD6，外接 USB-TTL 使用 3.3 V 电平。
- 确认 ST-LINK V2 使用 SWD 并连接 NRST。
- 确认外部 12 V 经降压后以稳定 5 V 为开发板供电，各设备共地且不并联其他供电源。
- 创建 `feature/m1-bare-metal-baseline` 分支，并提交 M1 硬件基线文档 `65cd01c`。
- 在 STM32CubeMX 中选择 STM32F407VGT6、LQFP-100，生成最小 HAL + CMake 裸机工程。
- 完成 M1 配置层核对：HSE 为 8 MHz，PLL 系统时钟为 168 MHz，APB1 为 42 MHz，APB2 为 84 MHz。
- 已生成 PB2 USER_LED GPIO 输出配置，以及 USART2 PD5/TX、PD6/RX、115200、8N1 配置。
- 使用 `CMake + Ninja + GNU Arm GCC` 成功完成首次固件编译、汇编、链接和产物生成。
- 已生成 .elf、.hex、.bin 构建产物；build/ 已由 .gitignore 正确排除，不纳入版本控制。
- CubeMX 裸机工程已形成独立 Git checkpoint：`feat: add STM32F407VGT6 CubeMX bare-metal project`。
- `feature/m1-bare-metal-baseline` 已推送到远程并建立对应 upstream。
- 使用 ST-LINK 成功将当前裸机固件下载到 STM32F407VGT6，并完成真实目标板运行验证。
- PB2 USER_LED 已完成最小 heartbeat 验证，按照程序设定的 500 ms HAL 延时周期稳定翻转。
- PB2 heartbeat 在连续 3 次 NRST 后均能够正常恢复运行。
- USART2 已通过 PD5/TX、115200、8N1、无流控配置完成真实 TX 验证。
- PC 通过 3.3 V USB-TTL 实际接收到 `USART2 TX OK` 启动信息。
- USART2 TX 在连续 3 次 NRST 后均能够重新输出，且 PB2 heartbeat 同时保持正常。
- 完成连续 3 次完全断电重新上电验证，每次 PB2 heartbeat 和 USART2 TX 均能够恢复正常。
- 当前 M1 已达到可构建、可下载、可运行、可观测和可重复启动的板级功能状态。
- M1 已完成最终 Git 收尾并形成 `0b6c86f`：`docs: close M1 and prepare M2 handoff`。
- 已创建 `feature/m2-freertos-baseline` 分支并从 M1 Known Good Baseline 开始 M2。
- 将 HAL Timebase Source 从 SysTick 迁移到 TIM6，TIM6 提供 HAL 1 ms 时基，SysTick 用于 FreeRTOS Tick。
- 在 CubeMX 中启用 FreeRTOS，接口采用 CMSIS-RTOS2。
- 已加入 FreeRTOS Kernel、CMSIS-RTOS2 wrapper、ARM Cortex-M4F GCC port、`heap_4` 和 `FreeRTOSConfig.h`。
- FreeRTOS Tick 当前为 1000 Hz。
- 建立 `HeartbeatTask`，使用 PB2 USER_LED 和 `osDelay(500U)` 完成单用户任务调度验证。
- FreeRTOS 单任务版本已成功完成 CMake + Ninja + GNU Arm GCC 构建、链接、烧录和真实目标板运行。
- 单任务版本 NRST ×3 和完全断电重新上电 ×3 均通过。
- 已形成 `4d62a23`：`feat: establish FreeRTOS single-task baseline`。
- `feature/m2-freertos-baseline` 已推送到远程并建立 upstream。
- 已新增 `UARTTask`，周期通过 USART2 输出测试字符串并使用 `osDelay(1000U)` 阻塞。
- `HeartbeatTask` 与 `UARTTask` 当前均为 `osPriorityNormal`。
- 两个同优先级用户任务已经在真实硬件上完成稳定共存运行验证。
- 双任务版本 NRST ×3 和完全断电重新上电 ×3 均通过。
- 已形成并推送 `a8e588c`：`feat: establish FreeRTOS dual-task baseline`。
- 当前 M2 已达到可构建、可烧录、可启动、可观测并可重复运行的 FreeRTOS 双用户任务基线状态。
- 已通过最小实验验证不同优先级任务抢占行为，高优先级 UARTTask 能够在从 Blocked 恢复为 Ready 后抢占持续运行的低优先级任务。
- 已形成 `46aef6a：feat: verify FreeRTOS priority preemption`。
- 已使用 osThreadGetStackSpace() 完成 HeartbeatTask 和 UARTTask 的运行时 Stack High Water Mark 验证。
- UARTTask 初始 512 B Stack 历史最小剩余仅 64 B，因此根据实际测量将 Stack 调整至 1024 B。
- UARTTask 调整至 1024 B 后，在当前完整运行路径下历史最小剩余 Stack 稳定约为 560 B。
- 已使用 xPortGetFreeHeapSize() 和 xPortGetMinimumEverFreeHeapSize() 完成 FreeRTOS Heap 运行时观测。
- 当前 configTOTAL_HEAP_SIZE 为 15360 B，Current Free Heap 和 Minimum Ever Free Heap 均稳定为 13592 B。
- 已形成 `51f143c：feat: add FreeRTOS runtime resource monitoring`。
- 已核对 configUSE_PREEMPTION = 1。
- 已确认 configUSE_TIME_SLICING 当前最终有效值为 1。
- 已建立两个相同 osPriorityNormal 优先级、持续 Ready 的 Busy Task，完成同优先级时间片调度行为验证。
- BusyTaskA 和 BusyTaskB 的计数器均持续增长，证明两个同优先级持续 Ready Task 均能够持续获得 CPU 执行机会。
- 同优先级时间片实验连续 3 次 NRST 均能够重复正常运行。
- 时间片实验完成后已删除 BusyTask 测试结构并恢复正常周期任务 baseline。
- 当前最终任务结构为 HeartBeatTask + UARTTask，两个任务均使用 osPriorityNormal 并通过 osDelay() 主动阻塞。
- HeartBeatTask 使用 512 B Stack，最终 baseline 历史最小剩余约 384 B。
- UARTTask 使用 1024 B Stack，最终 baseline 历史最小剩余约 560 B。
- 最终 baseline 中 FreeRTOS Current Free Heap 和 Minimum Ever Free Heap 均稳定为 13592 B。
- M2 Final Baseline 已完成 NRST ×3 和完全断电重新上电 ×3 验证。
- 未观察到 HardFault、Hang、任务异常停止或 Heap 持续下降。
- 已清理 CubeMX/编辑器产生的大量 CRLF/LF EOL-only 工作区噪音，最终功能修改边界仅保留 .ioc 与 Src/freertos.c。
- 已形成 `9c7333b：feat: finalize FreeRTOS scheduling baseline`。
- 当前 M2 已达到可构建、可烧录、可运行、可观测、可重复启动并具有基础运行时资源监测能力的稳定 FreeRTOS baseline。
- M2 已通过 PR 合并到 main，Merge Commit 为 `72ced0a`。
- 已创建 `feature/m3-debug-infrastructure`，从最新 M2 稳定 main 开始 M3。
- 已建立 Inc/debug_log.h 和 Src/debug_log.c 独立日志模块。
- 日志模块当前通过 DebugLog_Init(&huart2) 注入 USART2 Handle。
- 已建立 DEBUG、INFO、WARN、ERROR 四级日志接口。
- 已建立 DEBUG_LOG_ENABLE 全局编译期开关。
- 已建立基于预处理器的日志等级编译期裁剪。
- 日志模块当前使用 128 B 静态共享 buffer。
- 已使用 CMSIS-RTOS2 Mutex 保护共享日志 buffer 和 USART2 阻塞发送完整事务。
- FreeRTOS UARTTask 原有直接 HAL_UART_Transmit() 和 snprintf() 日志路径已经迁移到统一 LOG_INFO()。
- 已形成并推送 `6a760d3：feat: establish debug logging infrastructure`。
- 已形成并推送 `4701c62：feat: route FreeRTOS runtime output through debug logger`。
- 已完成 DEBUG_LOG_ENABLE = 1 / 0 clean build 对比。
- 完整日志开启时构建结果为 FLASH 27424 B、RAM 21056 B。
- 完整日志关闭时构建结果为 FLASH 21316 B、RAM 20512 B。
- 当前完整日志调用链的实测构建差值约为 FLASH +6108 B、静态 RAM +544 B。
- DEBUG / INFO / WARN / ERROR 四轮实际板级输出均与预期过滤结果一致。
- 已完成两个 osPriorityNormal Task、约 10 ms 周期长日志并发压力测试。
- 当前并发压力测试条件下未观察到日志交叉、payload 混合、乱码、Hang 或 HardFault。
- M3 Final Baseline 中 HeartBeatTask 历史最小剩余 Stack 稳定为 384 B。
- M3 Final Baseline 中 UARTTask 历史最小剩余 Stack 稳定为 512 B。
- M3 Final Baseline 中 FreeRTOS Current Free Heap 和 Minimum Ever Free Heap 均稳定为 13504 B。
- 相比 M2 Final Baseline，UARTTask 历史峰值 Stack 使用增加约 48 B，FreeRTOS Heap 额外消耗约 88 B。
- M3 Final Baseline 已完成 NRST ×3 和完全断电重新上电 ×3。
- 六次重新启动后均能恢复正常日志、Heartbeat 和运行时资源观测。
- 未观察到 HardFault、Hang、日志异常停止或 Heap 持续下降。
- 当前 M3 已达到可构建、可烧录、可运行、可观测、可裁剪、可重复启动并具有共享 UART 日志互斥保护的稳定状态。
- 已形成并推送 M3 文档关闭 checkpoint `75523b6：docs: close M3 and prepare M4 handoff`。
- 项目路线已扩展为 M0～M15：M10 新增 CAN，M11 新增 Bootloader / IAP，M12 新增 Watchdog / Reset Cause / Fault Diagnostics，原 M10～M12 顺延为 M13～M15。
- M3 已通过 PR 合并到 `main`，Merge Commit 为 `c13b8e5`。
- 已从最新 M3 稳定 `main` 建立 `feature/m4-rtos-communication-baseline`。
- 已形成并推送 `ed6c3a7：docs: update roadmap for expanded project milestones`。
- 已建立独立 M4 IPC 模块：
  - `Inc/m4_ipc.h`
  - `Src/m4_ipc.c`
- `Src/freertos.c` 当前仅调用 `M4Ipc_Init()` 启动 M4 IPC 模块，Queue、Task Handle 和具体通信逻辑保持在 `m4_ipc.c` 内部。
- M4 用户源码通过根目录 `CMakeLists.txt` 加入构建，没有修改 CubeMX 生成的 `cmake/stm32cubemx/CMakeLists.txt`。
- 已建立 `M4QueueMessage_t`，当前包含 `sequence`、`value` 和 `tick` 三个 `uint32_t` 字段，消息大小为 12 B。
- 已建立长度为 8 的 `M4DataQueue`。
- 已建立 `M4ProducerTask` 和 `M4ConsumerTask`。
- 当前 Queue Final Baseline：
  - Producer Period：200 ms。
  - Producer Priority：`osPriorityNormal`。
  - Producer Stack：1024 B。
  - Consumer Priority：`osPriorityNormal`。
  - Consumer Stack：1024 B。
  - Queue Put Timeout：0。
  - Consumer Queue Get Timeout：`osWaitForever`。
- 已通过真实 STM32F407VGT6 验证结构体按值进入 Queue，Producer 后续修改自己的局部对象不会修改已经入队的数据副本。
- 正常路径下已连续观察到 `seq=1,2,3...`，`value = sequence × 10`，Tick 基本每 200 tick 增加一次。
- 正常路径下 Queue Get 后 Count 持续为 0，未观察到 Queue Full 或 Sequence Gap。
- 已完成 Queue Full / Backpressure 人工压力实验：
  - Producer 临时调整为约 50 ms。
  - Consumer 临时增加约 500 ms 处理延迟。
  - Queue Count 实际达到 8。
  - `osMessageQueuePut()` 在 Queue Full 且 timeout = 0 时立即失败。
  - Drop Counter 持续增加。
  - Consumer 在旧 Queue 库存消耗后能够检测到 Sequence Gap。
- 已验证 Queue Full 不会破坏此前已经成功入队的消息，只会使新的 Put 失败。
- 已验证固定长度 Queue 只能吸收短时间突发，不能解决长期 Producer Throughput 高于 Consumer Throughput 的问题。
- 压力测试中发现 M4ProducerTask 初始 512 B Stack 历史最小剩余降至 0 B。
- 已确认 Queue Full 后的格式化日志路径会显著增加 Producer 历史峰值 Stack 使用。
- M4ProducerTask Stack 已从 512 B 调整为 1024 B。
- 调整后以相同压力路径重新验证：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：496 B。
  - M4ProducerTask 历史最小剩余 Stack：512 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - FreeRTOS Current Free Heap：11048 B。
  - FreeRTOS Minimum Ever Free Heap：11048 B。
- Producer Stack 增加 512 B 后 FreeRTOS Free Heap 同样减少 512 B，运行时资源变化与配置一致。
- 压力实验结束后已经恢复 200 ms Producer 和无人工 Consumer Delay 的正常 baseline。
- 恢复后的 M4.1 Final Baseline 已通过稳定性验证。
- 未观察到 Queue Full、Sequence Gap、Heap 持续下降、Hang 或 HardFault。
- M3 HeartBeatTask、UARTTask 和 Debug Logger 在 M4.1 实验期间持续正常运行。
- 已形成并推送 `b78282c：feat: establish RTOS queue communication baseline`。
- 已建立 M4.2 Task → Task CMSIS-RTOS2 Thread Flags 通知链路。
- 已建立 `M4EventTask` 和 `M4WorkTask`。
- 已定义 `M4_EVENT_WORK_READY` 单一事件位。
- M4EventTask Final Baseline：
  - Priority：`osPriorityNormal`。
  - Stack：1024 B。
  - Period：1000 ms。
- M4WorkTask Final Baseline：
  - Priority：`osPriorityNormal`。
  - Stack：1024 B。
  - 使用 `osThreadFlagsWait(..., osFlagsWaitAny, osWaitForever)` 阻塞等待。
- 已验证 Work 在没有事件时阻塞，在 Event Producer Set Flag 后正常被唤醒。
- 正常路径已验证 11 Sets → 11 Receives。
- 原 M4 Queue 在 Thread Flags 实验期间继续保持 Sequence 连续和正常消费。
- 已完成人工重复同一 Flag bit 的事件合并实验。
- Burst 实验中临时提高 Event Producer Priority，并在 Work 运行前连续 Set 相同 Flag 3 次。
- 实际观察到 33 Sets → 11 Receives。
- 已确认相同 Thread Flag bit 的重复 Set 表达 Pending State，不保存独立事件次数。
- 已确认 Thread Flags 不能作为无损事件计数器使用。
- 已明确当前职责边界：
  - Queue：结构化数据传递和有限消息缓冲。
  - Thread Flags：轻量事件状态通知。
- Burst 压力路径中 M4EventTask 历史最小剩余 Stack 为 520 B。
- Final Baseline 中：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：504 B。
  - M4ProducerTask 历史最小剩余 Stack：808 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - M4EventTask 历史最小剩余 Stack：528 B。
  - M4WorkTask 历史最小剩余 Stack：528 B。
  - FreeRTOS Current Free Heap：8776 B。
  - FreeRTOS Minimum Ever Free Heap：8776 B。
- M4.2 Final Baseline 已完成 NRST ×3。
- M4.2 Final Baseline 已完成完全断电重新上电 ×3。
- 六次重新启动后 Queue、Thread Flags、Heartbeat、UART、Stack 和 Heap 均能够稳定恢复。
- 未观察到 HardFault、Hang 或 Heap 持续下降。
- M4.2 最新功能 checkpoint：`<fd80cc9>`，`feat: establish RTOS thread flag notification baseline`。
- 已完成 M4 Binary Semaphore Task → Task 同步实验。
- 已建立 `SyncSem`：
  - Maximum Count：1。
  - Initial Count：0 / Depleted。
- 已建立 `SemProducerTask` 和 `SemConsumerTask`。
- Consumer 使用 `osSemaphoreAcquire(..., osWaitForever)` 阻塞等待。
- Producer 每约 2000 ms Release Semaphore。
- 已验证 Producer Release 后 Consumer 能够稳定被唤醒，并在 Acquire 后重新进入 Waiting。
- 已形成并推送 `435ba4f：feat: establish RTOS semaphore synchronization baseline`。
- 已将 `debug_log.c` 从 CubeMX 生成 CMake source list 的依赖中移出，由根目录 `CMakeLists.txt` 显式维护。
- 已形成并推送 `34ddce3：build: keep debug logger outside CubeMX generated sources`。
- 已建立 `SharedDataMutex`。
- 已建立 `MutexTaskA` 和 `MutexTaskB`。
- 已使用 `shared_value` 验证两个 Task 的共享数据 Read-Modify-Write 互斥访问。
- 已增加 `shared_op_seq` 区分真实临界区执行顺序和 UART 日志打印顺序。
- 已验证按 `shared_op_seq` 检查时共享数据更新连续，无旧值覆盖。
- 已形成并推送 `c5bc54a：feat: establish RTOS mutex protection baseline`。
- Queue、Thread Flags、Binary Semaphore、Mutex 已完成联合长期运行验证。
- 初始同步 Task Stack 测量发现：
  - SemProducerTask 最小剩余：32 B。
  - SemConsumerTask 最小剩余：32 B。
  - MutexTaskA 最小剩余：8 B。
  - MutexTaskB 最小剩余：8 B。
- 已根据真实 High Water Mark 调整 Stack：
  - Semaphore Task：约 512 B → 768 B。
  - Mutex Task：约 512 B → 1024 B。
- 调整后长期运行：
  - SemProducerTask 最小剩余：288 B。
  - SemConsumerTask 最小剩余：288 B。
  - MutexTaskA 最小剩余：488 B。
  - MutexTaskB 最小剩余：520 B。
  - FreeRTOS Current Free Heap：4568 B。
  - FreeRTOS Minimum Ever Free Heap：4568 B。
- Stack 总增量约 1536 B，Free Heap 同样减少约 1536 B，资源变化符合预期。
- M4.3 阶段历史构建（不代表最新 M4.5）：
  - RAM：21112 B / 128 KB，16.11%。
  - FLASH：33724 B / 1 MB，3.22%。
- 已形成并推送 `c84fed8：fix: increase M4 synchronization task stack margins`。
- 当前未观察到 HardFault、Hang、Heap 持续下降、Queue Sequence Gap、Semaphore 死锁或 Mutex 数据覆盖。
- 已建立独立 M4 Ring Buffer 模块：
  - `Inc/ring_buffer.h`
  - `Src/ring_buffer.c`
- 已通过根目录 `CMakeLists.txt` 加入 Ring Buffer 构建。
- 旧版测试文档将 Ring Buffer 记为固定 8 B 数组、有效容量 7 B；后续发现与已上传源码快照的 `uint32_t buffer[8]` 不一致，需依据最新头文件核对元素宽度与字节数。
- 已实现 RingBuffer_Init / WriteByte / ReadByte / IsEmpty / IsFull。
- 使用 head / tail 索引及预留一个空槽区分 Empty 和 Full。
- Full 时拒绝新数据写入，并通过 `overflow_count` 记录拒绝次数。
- 已完成 Empty / Full / Full Read / Wrap Write / Wrap Read 确定性板级测试。
- 五个测试阶段全部 PASS。
- 已验证 Write Index 和 Read Index 循环回绕。
- 已验证 FIFO 数据顺序与空读输出变量保护。
- 实测溢出计数为 1。
- 最终读写索引为 head=1、tail=1。
- 已观察到 Queue、Thread Flags、Mutex 和 UART 在同一测试固件中持续输出。
- 本轮 FreeRTOS Heap=4568 B、MinHeap=4568 B。
- 2026-10-16 Ring Buffer 测试固件曾临时复用 SemProducerTask / SemConsumerTask；该临时状态已在后续 Semaphore 恢复中结束。
- 2026-10-16 的 Ring Buffer 测试本身未覆盖 Semaphore 回归；后续已单独通过。
- 已形成功能 checkpoint：
  - `f8c3e52`：`feat: establish M4 ring buffer baseline`。
- `f8c3e52` 已成功推送至远程 M4 功能分支。
- M4.4 Ring Buffer 基础功能已完成，ISR / Task 并发安全仍未验收。
- Semaphore 已由 Ring Buffer 临时测试入口恢复并完成真实板级功能回归，形成并推送 `c4c0721`。
- 已使用 TIM7 完成 M4.5 实际 IRQ → `osThreadFlagsSet()` → Worker 阻塞唤醒链路。
- 已在 TIM7 优先级 5、FreeRTOS 阈值 5、4 位抢占优先级分组条件下完成 ISR-safe API 约束核对。
- 已观察 `irq=30/40/50/60` 对应 `received=30/40/50/60`、`errors=0`。
- 与 M4ISR 并行运行时原 Queue、Semaphore、Mutex、Task Flags 和 UART 继续正常输出。
- 新增 ISR Worker 后测得 Stack 历史最小剩余 520 B，Heap/MinHeap 3432 B；用户确认后续持续运行测试通过。
- M4.5 独立功能 checkpoint：`adf5d5e`，已推送远程；用户确认本地分支 clean。

## In Progress

- M4.1～M4.5 的基础功能及指定实验已完成；M4.5 测试记录和文档收尾正在处理。
- 下一任务：M4 Final Integration，验证所有保留的正式通信机制在同一当前固件中的职责边界和联合运行。
- 进一步确定 Ring Buffer 的保留测试模式与元素类型；测试逻辑不能占用正式 Semaphore 任务。
- 对 Queue、Thread Flags、Semaphore、Mutex、Ring Buffer 基本功能和 TIM7 ISR 通知进行完整回归，记录完整构建资源、Stack / Heap、长期稳定性以及 NRST ×3 / 断电重启 ×3。
- 再决定是否关闭 M4，不能直接将 2026-10-20 的 M4.5 通过等同于 M4 Final Integration 完成。

## Blocked

当前无已确认的外部阻塞。M4 Final Integration 所需的最终联合回归与重启测试尚待执行。

## Known Bugs

当前日志中无已确认且尚未解决的固件运行 Bug。

- M4.1 Queue Producer 历史 Stack 不足、M4.3 同步任务 Stack 余量不足，已在既有 checkpoint 调优。
- M4.4 Ring Buffer Consumer 曾因未检查读取返回状态重复打印旧值，已修复并有确定性测试记录。
- CubeMX Generate Code 曾将 LF 工作区文件改为 CRLF，导致大量 Git 工作区噪声；已用字节级定向 CRLF → LF 方式恢复，仍属于每次重新生成后要检查的开发流程风险。
- TIM7 早期生成的 ARR 为 9，与目标 999 不一致；随后确认 `.ioc` 为 `TIM7.Period=999`、重新生成的 `Src/tim.c` 为 999，已解决。
- Ring Buffer 文档的数据类型/字节容量与先前上传的源码快照不一致，属于待复核的文档一致性问题，而非已证实的运行 Bug。
- Ring Buffer SPSC 并发安全、ISR/Task 共享数据以及 DMA Ownership 均未验证；不是当前 M4.5 已完成能力。
- 当前同步 `LOG_xxx()` 不是 ISR-safe。

## Current Branch

- 当前 Branch：`feature/m4-rtos-communication-baseline`。
- Upstream：`origin/feature/m4-rtos-communication-baseline`。
- 最新已确认 HEAD：`adf5d5e`：`feat: establish M4 ISR to task notification baseline`。
- 本地 `HEAD` 与 `origin/feature/m4-rtos-communication-baseline` 均为 `adf5d5e`，用户提供的 `git status` 为 `nothing to commit, working tree clean`。
- Semaphore 恢复：`c4c0721`：`fix: restore M4 semaphore task baseline`。
- Ring Buffer 文档：`5b0240b`：`docs: record M4 ring buffer baseline verification`。
- Ring Buffer 功能：`f8c3e52`；M4 同步文档：`5c9efac`；Stack 调优：`c84fed8`；Mutex：`c5bc54a`；Semaphore：`435ba4f`。
- M4.5 功能提交已同步远程；**此 ZIP 的文档更新尚未应用到本地仓库、未提交或推送**，不得假设出现新的 docs commit。

## Last Verified Function

当前最新功能验收：M4.5 TIM7 ISR → Thread Flags → Worker Task。

- TIM7 配置：输入定时器时钟 84 MHz，PSC=8399，ARR=999，预期 100 ms / 10 Hz。
- 优先级：TIM7 NVIC Preemption=5，`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5`，`configPRIO_BITS=4`，NVIC Grouping 为 4 bits 抢占优先级。
- TIM6 HAL 1 ms Timebase 不受 TIM7 回调影响：`HAL_IncTick()` 仅 TIM6 分支执行。
- M4.5 真实日志采样：

```text
[I][M4ISR] irq=30 received=30 errors=0
[I][M4ISR] irq=40 received=40 errors=0
[I][M4ISR] irq=50 received=50 errors=0
[I][M4ISR] irq=60 received=60 errors=0
```

- 当前低负载窗口各采样点相等；不推断高负载时 Thread Flags 不会合并事件。
- 原 Queue 的日志 `seq=12..31` 连续、`value = seq × 10`、`count=0`；Semaphore 持续释放/获取；Mutex 临界区 `seq=3..6`、共享值连续；原 M4F Set/Receive 对应；UART 正常。
- M4ISR 与原 M4 基线联合运行期间的资源采样：

```text
HeartFree=384 B     UARTFree=480 B
M4ProdFree=808 B    M4ConsFree=496 B
M4EvtFree=528 B     M4WorkFree=528 B
SemProFree=288 B    SemConFree=288 B
MutexAFree=520 B   MutexBFree=520 B
ISRWorkerFree=520 B
Heap=3432 B        MinHeap=3432 B
```

- 与添加 M4ISR Worker 前的 `Heap=4568 B` 相比，运行时当前 Free Heap 减少 1136 B；Worker 配置 Stack 1024 B。
- 本阶段的干净构建记录：FLASH 34748 B（3.31%），RAM 21192 B（16.17%）；不等同于动态 Heap 使用。
- 用户随后确认持续运行测试均通过；本次提供的材料没有附上最终结束时间/完整末尾计数，不虚构具体分钟数或最终 IRQ 总数。
- 没有证据表明本次进行了 **M4 Final Integration** 所需的 NRST ×3 / Power-cycle ×3、Ring Buffer SPSC 并发压力测试。
- 最新确认功能提交：`adf5d5e`，并已推送远程。

## Next Step

1. 将本次 ZIP 中九份 Markdown 文档覆盖/合并回本地 `docs/`，检查 diff 和事实准确性。
2. 单独提交 M4.5 文档 checkpoint（提交号尚未产生），保留 `adf5d5e` 功能提交不变。
3. 进入 **M4 Final Integration**：整理临时 Ring Buffer 测试入口，复核实际 `ring_buffer.h` 与文档类型/容量一致性，确定当前正式保留功能。
4. 完整回归 Queue / Task Flags / Semaphore / Mutex / Ring Buffer Basic / TIM7 ISR → Task；注意 Ring Buffer Basic != SPSC 并发安全。
5. 记录干净构建、FreeRTOS Stack High Water Mark、Current Free Heap、Minimum Ever Free Heap 和异常路径结果。
6. 执行 M4 Final Baseline 长时间运行、NRST ×3、完全断电重启 ×3，按真实证据标记通过与否。
7. 检查 `.ioc` / CubeMX 生成代码与独立用户模块的边界，避免无意义的 CRLF/LF diff；记录 Commit/PR 并准备 M4 关闭。
8. **M4 关闭前不启动 M5 ADC + Timer + DMA 业务实现**。
