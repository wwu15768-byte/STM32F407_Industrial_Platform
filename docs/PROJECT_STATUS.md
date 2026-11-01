# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-11-01

## Current Milestone

M6：USART2 DMA + IDLE 不定长通信。M0～M5 已完成基础里程碑；M5 已通过 PR #6 合并，用户在 M6 创建 `feature/m6-baseline`（起点 `30b0072`）。本轮 M6-P1 CubeMX 配置与 Git EOL 清理完成，M6-P2 Normal DMA + IDLE/TC 基础功能及队列压力对照已实测；M6-P3 Circular DMA + Ring Buffer 尚未开始。

- M6-P1：USART2 PD5/PD6、115200/8N1；RX DMA1 Stream5 Channel4、Normal、Byte；启用 DMA1 Stream5 和 USART2 IRQ。生成代码有效 Diff 收敛到五个文件，`git diff --check` 无输出。
- M6-P2：64 B Normal DMA `HAL_UARTEx_ReceiveToIdle_DMA`；IDLE（事件 2）与 TC（事件 0）经回调复制后交 CMSIS-RTOS2 Queue，UARTTask 消费。1/4/6/31/32/63/64 B、二进制顺序验证通过。
- 修复：此前从普通 RxEvent 回调误调用错误统计函数，已纠正；11 次 ASCII `01 02 03 04`（11 B）正常接收，`Count=11 Drop=0 StartFail=0`、`TotalError=0`。
- 压力对照：8 格 Queue `Count=747 Drop=49`、Heap=11704 B；64 格 Queue `Count=753 Drop=0`、Heap=7672 B；均 `StartFail=0`、USART Error 统计 0。本轮 Drop=0 仅说明软件入队无失败，不证明端到端字节完整。
- 范围边界：M6 本阶段不与 M5 ADC/DMA2 新回归绑定；不因此改写 M5 既有验收，也不宣称当前 M6 固件重新验证了 M5。

## Current Goal

完成 M6-P2 已验证基础能力的代码/文档 Git 归档后，进入 M6-P3 Circular DMA + 独立字节 Ring Buffer，解决 Normal DMA 重启空窗、回绕、连续输入和 Queue 背压。当前已验证的最小链路为：

```text
USART2 RX / PD6 / 115200 8N1
  -> DMA1 Stream5 Channel4 / Normal / 64 B
  -> HAL_UARTEx_RxEventCallback (IDLE/TC)
  -> Copy RX payload / osMessageQueuePut(timeout=0)
  -> CMSIS-RTOS2 Message Queue (8/64-depth experiments)
  -> existing UARTTask / Count / Size / Event / Drop / StartFail
```

ISR 不进行阻塞日志；现有 USART2 阻塞 TX Logger 保留任务上下文调用。后续需要记录实际发送字节数与 MCU 消费字节数、序列连续性及所有软件丢弃源。

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
- M4 最终验证已通过。

- 2026-10-28 M5-P0 Git 归档完成并远程同步：功能 `942fcc3`，文档 `e7f525c`；上一次确认的干净 HEAD 为 `e7f525c`，main 仍为 `e793039`。
- M5 新增 ADC1_IN1（PA1，10 kΩ 电位器）、TIM2 TRGO（84 MHz / 8399 / 199 = 50 Hz 设计值）、DMA2 Stream0 Circular（20×`uint16_t`），利用 Half/Full Callback 与 FreeRTOS `AcquisitionTask` 进行 10 点平均电压估算。
- 板级电压范围 `2～3287 mV`、最大端稳定测得 3287/3285/3286 mV，尚无万用表校准证据。
- DMA 回调与 Task Processed 计数对照：0、50 Tick 延时 Late=0；250 Tick 压力注入 Late=27→57→87，检测到超时风险；恢复正常后 Half=73/73、Full=72/72、Late=0、Error=0。
- 最新 Debug 增量构建成功（RAM 21448 B / 16.36%，FLASH 35680 B / 3.40%，CCMRAM 0），Heap/MinHeap=12368 B，HeartFree=384 B，UARTFree 最低可见 512 B。
- 用户确认当前 M5 采集固件长时间运行与 NRST ×3、Power-cycle ×3 均通过；时长未量化，不推断长期极端负载下逐样本无损。

- 2026-10-27 M5 PR #6 已合并：`30b0072` 是 `Merge pull request #6 from wwu15768-byte/feature/m5-adc-timer-dma-baseline`，M6 分支从该已合并基线创建。
- M6-P1 USART2 RX DMA + NVIC CubeMX 生成文件经 EOL 清理后仅 5 个有效改动，`git diff --check` 无输出。
- M6-P2 Normal DMA+IDLE/TC 1、4、6、31、32、63、64 B 以及二进制测试通过；11 次 ASCII 11 B 正常接收与纠正后的零错误统计通过。
- 8 格与 64 格 Queue 连续 ASCII 对照实验：最终 Drop 49→0，Heap 11704→7672 B；仅验证软件队列背压差异，仍未证明端到端零丢字节。

## In Progress

- M6-P2 已提供实机输出，但功能提交/文档提交、最新 Git HEAD / 工作区 clean 和 `cmake --build --preset Debug --clean-first` 最终终端输出尚未提供，先审核再归档。
- M6-P3 计划将 Normal DMA 改为 Circular DMA，建立独立 `uint8_t` Ring Buffer 和可核对的 EventBytes/BufferedBytes/ConsumedBytes/DropBytes、序列错误统计；尚未编码与验收。
- 真实 UART FE/ORE 故障注入及接收异常后恢复未做板级确认，可作为后续异常处理专项；本阶段不做 M5 ADC/DMA2 新回归。

## Blocked

当前没有已确认的外部硬件或工具阻塞；Git 审核与文档提交属于尚待完成的归档工作。

## Known Bugs

- 已修复：2026-11-01 M6 UART 错误统计函数误放在正常 RxEvent 中，导致 `TotalError` 误增。正常 ASCII 11 次收发后全部 `[RXE]` 计数为零；详见 `ERR-20261101-02`。
- 已复现：M6-P2 8 格消息队列在持续压力下 `Drop=49`，64 格在本轮相同形式负载下 `Drop=0`；属于被观测到的软件入队失败和容量/消费压力问题（`ERR-20261101-03`）。
- 已解决的工作区问题：CubeMX 再生成后的 CRLF/LF 噪音（`ERR-20261101-01`）。
- 尚未证实也尚未排除：DMA Normal 每次完成后重启空窗可能漏字节；`StartFail=0`、`TotalError=0`、Queue Drop=0 不能替代端到端字节完整性验证。

## Current Branch

- M6 当前开发分支：`feature/m6-baseline`，初始 HEAD `30b0072`，当时同为 `main` / `origin/main` 及 M6 远程起点，确认是 M5 PR #6 Merge Commit。
- 最近一次明确的 M6 `git status -sb` / Diff 验证发生在 P1 EOL 清理完成后：五个预期生成文件修改，未提交；后续 UART RX 业务代码、队列深度对照和文档的 Git 状态尚无新终端证明。
- **未提供 M6-P2 代码 Commit/Push、新 HEAD/clean 证据，不虚构提交号**。

## Last Verified Function

最新验收：**M6-P2 USART2 DMA Normal + IDLE/TC 不定长接收的功能基线，以及 8/64 格消息队列的背压对照**。

- 单包：已确认 1/4/6/31/32/63/64 B，包含 `00 55 AA FF` 等二进制，已测数据长度/顺序与 IDLE/TC 事件符合预期。31/63 B 为用户直接确认通过。
- 正常 ASCII：2026-11-01 发送 `01 02 03 04` 11 次，文本每次 11 B，正常接收十六进制 `30 31 20 30 32 20 30 33 20 30 34`，末次 `Count=11 Drop=0 StartFail=0`；全部 `[RXE]` 分类 0。
- 压力：每组 13 条可见长 ASCII Send，每条文本 3599 字符。8 格：`Count=747 Drop=49 StartFail=0`、Heap/MinHeap=`11704/11704 B`、UARTFree 最低 `344 B`；64 格：`Count=753 Drop=0 StartFail=0`、Heap/MinHeap=`7672/7672 B`、UARTFree 最低 `336 B`。队列增 56 项，Heap 差 4032 B；`Count` 已含测试前后窗口数据，不能用其直接定量说明字节丢失率。
- 本轮未测真实 FE/ORE 硬件故障、DMA 异步错误恢复、严格发送/消费总字节匹配、Circular DMA 回绕/覆盖。64 格测试仅验证观察窗口软件队列不丢弃，**不能声明 UART 端到端零丢字节**。
- 原 M5 测试记录均保留在 `TEST-20261028-*` / `TEST-20261029-*`，本轮 M6 无需新 ADC 回归。
- 详细记录：`TEST-20261101-01`～`TEST-20261101-04`；决策：`DEC-20261101-01`～`DEC-20261101-03`；故障：`ERR-20261101-01`～`ERR-20261101-03`；知识边界：`KG-015` / `KG-016`.

## Next Step

1. 保留并审核 M6-P2 已验证代码；执行 `git status -sb`、`git diff --stat`、`git diff --check`、`cmake --build --preset Debug --clean-first`，只记录实际成功的构建/提交结果。
2. 将 M6-P2 的 Normal DMA 功能基线与阶段压力测试文档分别暂存、检查 `git diff --cached --check` 和 `--stat`、提交并推送；不要将 M6-P3 尚未实现的能力写入功能提交。
3. 按 M6-P3 规划把 RX 改为 DMA Circular，不在正常 HT/TC/IDLE 后反复 Stop/Restart；为 UART 字节流建立独立于 M4 `uint32_t` 元素 Ring Buffer 的 `uint8_t` Ring Buffer。
4. 通过已知发送总字节数、序号/CRC 与 MCU 实际消费字节数验证 10 KB、50 KB 连续流，分别记录溢出、覆盖/序列错误和硬件 Error；发现失败时报告边界，而不是笼统写“完全无丢包”。
