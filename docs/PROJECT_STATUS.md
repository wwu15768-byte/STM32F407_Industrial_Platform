# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-10-01

## Current Milestone

M4：RTOS 通信、同步和数据缓冲。M0～M3 已完成，M4 为当前阶段。

M4.1 Queue Producer / Consumer 基线已经完成，最新功能 checkpoint：`b78282c`，`feat: establish RTOS queue communication baseline`。。当前准备继续 M4.2 Task Notification / CMSIS-RTOS2 Thread Flags，不进入 M5。

## Current Goal

在已经完成的结构化 Queue Producer / Consumer 基线上继续推进 M4，下一步通过 Task Notification / CMSIS-RTOS2 Thread Flags 建立轻量事件通知实验，并与 Queue 的“数据传递”职责进行对比。之后再进入 Binary Semaphore、Mutex 语义对比和 Ring Buffer。M4 完成前不提前实现 ADC + Timer + DMA、UART DMA、RS485、Modbus、CAN、Bootloader 或 Watchdog。

## Completed

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

## In Progress

M4.1 Queue Producer / Consumer checkpoint 已完成。当前准备进入 M4.2 Task Notification / CMSIS-RTOS2 Thread Flags，重点验证轻量事件通知、阻塞等待和任务唤醒行为，并与 Queue 的结构化数据传递职责进行对比。Binary Semaphore、Mutex 对比和 Ring Buffer 尚未开始。

## Blocked

无。

## Known Bugs

无已确认固件 Bug。

M4.1 压力测试曾发现 M4ProducerTask 初始 512 B Stack 在 Queue Full + 错误日志路径下历史最小剩余降至 0 B。该配置风险已经通过将 Producer Stack 调整为 1024 B 并重新压力验证解决，不作为当前 Known Bug。

当前 `LOG_xxx()` 仍只作为正常 Task / Thread context API，不是 ISR-safe 日志接口。

Task Notification、Binary Semaphore、Ring Buffer、ADC + Timer + DMA、UART DMA + IDLE、RS485、Modbus、CAN、Bootloader、Watchdog 和 Fault Diagnostics 属于尚未完成的后续功能，不作为 Known Bug。

## Current Branch

- 当前 Branch：`feature/m4-rtos-communication-baseline`。
- Upstream：`origin/feature/m4-rtos-communication-baseline`。
- 当前 HEAD：`b78282c`，`feat: establish RTOS queue communication baseline`。
- M4 路线更新 checkpoint：`ed6c3a7`，`docs: update roadmap for expanded project milestones`。
- M4 起点：`c13b8e5`，M3 Merge Commit。
- 当前 `main` / `origin/main` 位于 `c13b8e5`，包含完整 M0～M3。
- 当前 M4 功能分支已经推送并与远程同步。
- Working Tree：clean。

## Last Verified Function

- M4.1 RTOS Queue Communication Baseline 已经通过 STM32F407VGT6 真实硬件验证：
  - `M4ProducerTask` 能够周期生成 `M4QueueMessage_t`。
  - `M4DataQueue` Length = 8。
  - Queue Item Size = 12 B。
  - 消息采用结构体按值复制，不使用指针传递。
  - `M4ConsumerTask` 使用 `osMessageQueueGet(..., osWaitForever)` 阻塞等待消息。
  - Final Baseline Producer Period = 200 ms。
  - Queue Put Timeout = 0。
  - 正常路径 Sequence 连续。
  - 正常路径未观察到 Queue Full。
  - 正常路径未观察到 Sequence Gap。
  - 已人工验证 Producer 长期快于 Consumer 时 Queue Occupancy 会增加并最终达到 Full。
  - Queue Full 后 Put 能立即失败并增加 Drop Counter。
  - Queue 中已经成功入队的数据不会因后续 Put Failure 被破坏。
  - Consumer 能在旧库存消耗后检测到 Sequence Gap。
  - M4ProducerTask Stack 已根据压力测试由 512 B 调整为 1024 B。
  - 压力路径下 M4ProducerTask 历史最小剩余 Stack：512 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - FreeRTOS Current Free Heap：11048 B。
  - FreeRTOS Minimum Ever Free Heap：11048 B。
  - M3 HeartBeatTask、UARTTask 和 Debug Logger 保持正常。
  - Final Baseline 稳定性测试通过。
  - 当前最新功能 checkpoint：`b78282c`。

当前证据不包含 Task Notification、Binary Semaphore、Ring Buffer、ISR → Task 通知、USART2 RX、ADC + Timer + DMA、UART DMA、RS485、Modbus、CAN、Bootloader 或 Watchdog。

## Next Step

进入 M4.2 Task Notification / CMSIS-RTOS2 Thread Flags。

先建立最小 Task → Task 事件通知实验，只传递“事件发生”语义，不通过 Notification 复制结构化数据；验证发送、阻塞等待、唤醒和重复通知行为，并与已经完成的 Queue 数据传递模型进行对比。

M4.2 验证通过并形成独立 Git checkpoint 后，再进入 Binary Semaphore。暂不进入 M5 ADC + Timer + DMA。
