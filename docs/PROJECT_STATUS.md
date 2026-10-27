# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-10-27

## Current Milestone

M4：RTOS 通信、同步和数据缓冲（**功能验证基本完成；独立 SPSC 分支已提交并推送，里程碑尚未正式关闭**）。M0～M3 已完成。

- M4.1 Queue：完成。
- M4.2 CMSIS-RTOS2 Thread Flags Task → Task：完成，含重复 Flag 合并验证。
- M4.3 Binary Semaphore / Mutex：完成基础与联合回归。
- M4.4 Ring Buffer：Empty / Full / FIFO / Wrap-around 基础功能 PASS；2026-10-27 **Task ↔ Task SPSC 50,000 元素并发专项 PASS**。
- M4.5 TIM7 ISR → Worker：100 ms / 10 Hz 通知链路及持续运行基线 PASS。
- M4 Final Integration：临时同机多模块联测及用户确认的长期运行已通过；SPSC 专项在独立分支通过。尚需 SPSC OFF 正式固件回归、M4 最终 NRST ×3 / Power-cycle ×3 的证据、PR 合并和文档关闭。
- M5 ADC + Timer + DMA：未开始。

## Current Goal

完成 SPSC 专项分支并入 M4、最终正式固件验收及 M4 里程碑关闭，不重复已完成的 M4.1～M4.5 基础功能。

已验证的 SPSC 独立运行数据链路：

```text
SpscProducerTask (uint32_t 1..50000)
  -> Ring Buffer [8 个 uint32_t，预留 1 槽；有效 7 元素]
  -> SpscConsumerTask (校验连续、无重号跳号)
```

- `head/tail` 使用 C11 `_Atomic` 与 acquire/release 内存序；仅有一个 Producer 修改 head、一个 Consumer 修改 tail。
- Full 时拒绝写入并重试同一元素，Full 尝试 `7064` 次；Empty 读取重试 `7419` 次；最终 `50000` 发 / `50000` 收，`overflow_count=7064`。
- Worker Stack 采样：SPSC Producer `876 B`，Consumer `444 B`，ISR Worker `520 B`；Heap / MinHeap `3432 B`。
- 本次测试只证明 Task ↔ Task SPSC；不能推广为 ISR ↔ Task 或 DMA Buffer Ownership 正确。
- 分支：`feature/m4-spsc-concurrency-verification`，已知 HEAD/远程 `d93665e`；父级 M4 分支已知为 `405b9ab`，**没有收到 PR 已合并的确认**。

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

- 2026-10-27：M4 最终临时集成运行中，Ring Buffer 确定性测试 EMPTY/FULL/FULL READ/WRAP WRITE/WRAP READ 全部 PASS，之后原 Semaphore 与 Queue / M4F / Mutex / TIM7 ISR 继续并行运行；用户确认长期运行通过（准确时长及最终末尾计数未给出）。
- 2026-10-27：独立 SPSC 测试两 Task 共传输 50,000 个递增 `uint32_t`，最终 `PASS sent=50000 recv=50000 full=7064 empty=7419`、`overflow=7064 elapsedTicks=7501`；不是历史分阶段握手实验。
- SPSC 压力运行期间 Queue `seq=1..40` 连续，TIM7 ISR 可见至 `irq=70 received=70 errors=0`，Semaphore / Mutex / M4F 正常；Heap / MinHeap=3432 B，SPSC P/C Stack 剩余 876/444 B。
- SPSC 功能 checkpoint：`d93665e` 已在独立分支提交并推送，用户确认工作区 clean；父 M4 文档 checkpoint 为 `405b9ab`。

## In Progress

- SPSC 测试作为可选构建代码保留，源文件 `Inc/m4_spsc_test.h` / `Src/m4_spsc_test.c`，通过 `M4_ENABLE_SPSC_TEST` 启用；需要确认 CMake Cache `OFF` 后正式基线 clean build、烧录、无 SPSC 日志的回归证据。
- 按 M4 关闭标准补齐 SPSC 版本 NRST ×3、完全断电重启 ×3；之前 M1～M3 的重启记录不替代本次最终固件测试。
- 将 `feature/m4-spsc-concurrency-verification` 经 PR 合入 `feature/m4-rtos-communication-baseline`，在合并后复核真实工作区、功能和资源。
- 更新 M4 最终集成报告与文档 Git checkpoint，随后评估 M4 → `main` 的 PR 合并与 M5 分支建立。

## Blocked

当前无确认的外部依赖阻塞；剩余项主要是实机复位/正式基线验证，以及 Git PR 与文档收尾，不能视为已完成。

## Known Bugs

当前日志中无已确认且尚未解决的固件运行 Bug。

- M4.1 Queue Producer 历史 Stack 不足、M4.3 同步任务 Stack 余量不足，已在既有 checkpoint 调优。
- M4.4 Ring Buffer Consumer 曾因未检查读取返回状态重复打印旧值，已修复并有确定性测试记录。
- CubeMX Generate Code 曾将 LF 工作区文件改为 CRLF，导致大量 Git 工作区噪声；已用字节级定向 CRLF → LF 方式恢复，仍属于每次重新生成后要检查的开发流程风险。
- TIM7 早期生成的 ARR 为 9，与目标 999 不一致；随后确认 `.ioc` 为 `TIM7.Period=999`、重新生成的 `Src/tim.c` 为 999，已解决。
- Ring Buffer 文档的数据类型/字节容量与先前上传的源码快照不一致，属于待复核的文档一致性问题，而非已证实的运行 Bug。
- Ring Buffer **Task ↔ Task SPSC** 已通过 50,000 元素专项验证；**ISR ↔ Task 共享 Ring Buffer、DMA Ownership、多生产者**仍未验证，属于适用边界而非已确认 Bug。
- 当前同步 `LOG_xxx()` 不是 ISR-safe。

## Current Branch

- 最新用户确认 Branch：`feature/m4-spsc-concurrency-verification`。
- Upstream：`origin/feature/m4-spsc-concurrency-verification`。
- HEAD / origin：`d93665e`：`feat: verify M4 ring buffer SPSC concurrency baseline`（用户明确确认工作区 clean）。
- 父级 M4 主开发分支：`feature/m4-rtos-communication-baseline`，最近已知 HEAD / origin 为 `405b9ab`：`docs: record M4 ISR notification baseline verification`。
- 历史 M4.5 功能 `adf5d5e`；Semaphore 恢复 `c4c0721`；Ring Buffer 基础功能 `f8c3e52`。
- **没有本次 SPSC → M4 PR 完成或 M4 → main 合并成功的证据**。本 ZIP 文档更新尚未写入用户仓库、尚无新的文档提交号。

## Last Verified Function

2026-10-27 最新专项：M4 Ring Buffer **Task ↔ Task SPSC**，同时保留 M4 联合通信验证。

- 硬件/系统：STM32F407VGT6 + FreeRTOS / CMSIS-RTOS2，Ring Buffer `uint32_t buffer[8]`，可用容量 7 元素，C11 `_Atomic head/tail`。
- 并发测试：SPSC Producer 与 Consumer 两独立任务；不以逐次 Flag 控制交换次序；接收侧逐项校验 1..50000。
- 实测日志：

```text
[I][SPSC] START total=50000 capacity=7 elements
[I][SPSC] PASS sent=50000 recv=50000 full=7064 empty=7419
[I][SPSC] overflow=7064 elapsedTicks=7501
[I][SYS] SpscPFree=876 B SpscCFree=444 B
[I][SYS] M4EvtFree=528 B M4WorkFree=528 B Heap=3432 B MinHeap=3432 B
[I][SYS] ISRWorkerFree=520 B
```

- 同机回归观察：M4Q `seq=1..40` 连续，M4F 事件成对接收，Semaphore Release/Acquire 持续，Mutex 共享数值按 A:+1/B:+10 变化；TIM7 统计 `irq=70 received=70 errors=0`（日志截取窗口）。
- 结论：本次 50,000 个数据未出现顺序校验失败，Full/Empty 均出现，测试程序报告 PASS；`overflow=7064` 是满时失败写入尝试，不是 7064 个最终丢失数据。
- 资源：SYS 采样中 Heap/MinHeap 均为 3432 B，SPSC Consumer Stack 剩余 444 B；不能将 7.501 秒的专项测试误写成长期工业级压力证明。
- 与之前暂时将 Ring Buffer 基础测试放在 Semaphore 任务中的联合实验不同；原联合运行用户确认长期通过，记录为**另一项**测试，不混成无阶段同步 SPSC。
- 运行范围：Task ↔ Task 单生产者/单消费者，不覆盖 ISR ↔ Task、DMA 和多生产者访问。
- 提交：`d93665e` 已推送独立 SPSC 分支；合并与正式固件最终验收待执行或待提交证据。

## Next Step

1. 如尚未完成，补齐 SPSC 专项 NRST ×3、Power-cycle ×3 以及 SPSC PASS 后其余 M4 模块继续稳定运行的实测证据。
2. 显式设置 `cmake --preset Debug -DM4_ENABLE_SPSC_TEST=OFF`，执行 clean build、烧录正式固件，检查没有 `[SPSC] START` 且 Queue / Semaphore / Mutex / Thread Flags / TIM7 ISR 正常、资源稳定。
3. 审核 `d93665e` 的六个目标文件及原子内存序实现，创建并合并 SPSC → M4 PR（Base: M4；Compare: SPSC）；确认远程状态并 fast-forward 本地 M4 分支。
4. 合并后的正式 M4 分支再次构建/板级验收，归档 `TEST_RECORD.md` 和其他 docs 的最终事实与未覆盖边界，建立 docs checkpoint。
5. M4 最终验收通过后，通过 PR 将 M4 → `main`；**未确认前不得把 M4 写为完成**，不提前创建 M5 功能分支。
