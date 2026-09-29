# SESSION_HANDOFF.md

> 当前会话快照；会话切换前整体更新。

## 当前 Milestone

M4：RTOS 通信、同步和数据缓冲，准备正式开始。

M0、M1、M2、M3 均已完成。本次会话只调整项目路线和文档，没有开始 M4 功能实现。

## 当前 Branch 与 Git 基线

- 当前 Branch：`feature/m3-debug-infrastructure`。
- Upstream：`origin/feature/m3-debug-infrastructure`。
- M3 最终功能 checkpoint：`4701c62`，`feat: route FreeRTOS runtime output through debug logger`。
- M3 文档关闭 checkpoint：`75523b6`，`docs: close M3 and prepare M4 handoff`。
- 修改本次路线前，本地 M3 分支与远程 M3 分支同步。
- 当前 `main` / `origin/main`：`72ced0a`，已包含完整 M2，尚未包含 M3。
- 本次路线文档修改保持未提交，等待用户检查后决定是否 commit。

## 已完成并验证的内容

### M0～M2

- M0 开发工具、Git 和项目文档体系已完成。
- M1 STM32F407VGT6 裸机 Known Good Baseline 已完成真实板级验证。
- M2 FreeRTOS 启动、双任务、优先级抢占、同优先级时间片、Task Stack、FreeRTOS Heap 和复位稳定性验证已完成。
- HAL Timebase 使用 TIM6，SysTick 用于 FreeRTOS Kernel Tick。

### M3

- 已建立独立 `debug_log` 模块，UART Handle 通过 `DebugLog_Init(&huart2)` 注入。
- 当前日志后端为 USART2，支持 DEBUG / INFO / WARN / ERROR。
- `DEBUG_LOG_ENABLE` 和 `DEBUG_LOG_LEVEL` 支持编译期裁剪。
- 使用 128 B static shared buffer 和 CMSIS-RTOS2 Mutex 保护“格式化 + UART 阻塞发送”完整事务。
- 使用 `snprintf()` / `vsnprintf()` 形成日志文本，并定义 `DebugLogStatus_t` 错误码。
- UARTTask 的调试输出已统一迁移到 `LOG_INFO()`。
- 日志开启/关闭 clean build、四级过滤、双任务约 10 ms 高频长日志并发压力实验均已有测试记录。
- M3 Final Baseline：
    - HeartBeatTask：512 B Stack，历史最小剩余 384 B。
    - UARTTask：1024 B Stack，历史最小剩余 512 B。
    - FreeRTOS Current Free Heap：13504 B。
    - FreeRTOS Minimum Ever Free Heap：13504 B。
    - 完整日志开启：FLASH 27424 B，RAM 21056 B。
    - NRST ×3：通过。
    - 完全断电重新上电 ×3：通过。
    - 当前测试条件下未观察到日志交叉、乱码、Hang、HardFault 或 Heap 持续下降。

## 本次路线调整

- 项目定位扩展为 STM32F407VGT6 + FreeRTOS 的工业实时数据采集与多总线通信平台。
- M0～M3 历史保持完成，M4 成为当前阶段。
- M5 升级为 ADC + Timer + DMA 数据采集子系统。
- 新增 M10 CAN、M11 Bootloader / IAP、M12 Watchdog / Reset Cause / Fault Diagnostics。
- Old M10 上位机链路 → New M13。
- Old M11 系统联调 → New M14。
- Old M12 项目收尾 → New M15。
- Linux 仅作为开发与用户态辅助能力，不进入 STM32 里程碑，也不加入 Linux Kernel 主线。

## 当前明确未实现

- M4 的 Queue、Task Notification、Binary Semaphore 和 Ring Buffer 尚未开始实现。
- 当前 `LOG_xxx()` 不是 ISR-safe API，也没有异步 LogTask 或 UART DMA logging。
- USART2 RX、ADC + Timer + DMA、UART DMA + IDLE、RS485、Modbus RTU、统一数据模型、CAN、Bootloader、Watchdog、Reset Cause 和 HardFault Diagnostic 均未实现。
- 没有为未来功能预设引脚、采样通道、Timer、Flash 分区、收发器或额外硬件。

## 已确认的重要约束

- 固件外设层只使用 STM32 HAL，不使用 SPL；除非明确要求，不使用 LL。
- FreeRTOS 应用层继续使用 CMSIS-RTOS2。
- ISR 保持最小，不调用当前同步日志接口，不做复杂计算或阻塞处理。
- M4 先建立 Queue、Notification、Semaphore、Mutex 语义和 Ring Buffer 边界，再进入 M5。
- 后续严格按 M4 → M5 → M6 → M7 → M8 → M9 → M10 → M11 → M12 → M13～M15 推进。
- 计划项不能写成已实现；每个完成状态必须有构建、运行或测试证据。

## 最近一次有效功能测试

- `TEST-20260927-01`：M3 调试日志基础设施与编译期全局裁剪通过。
- `TEST-20260927-02`：DEBUG / INFO / WARN / ERROR 编译期过滤通过。
- `TEST-20260927-03`：双任务高频长日志 Mutex 并发压力测试通过当前条件。
- `TEST-20260927-04`：M3 Final Baseline 稳定性验证通过，NRST ×3 和 Power-cycle ×3 通过。

本次只修改文档，没有新增固件功能测试，也没有伪造测试结果。

## 下一步最合理的一个任务

1. 用户检查本次文档 diff，确认路线、状态和范围边界。
2. 用户决定是否创建文档 commit；本次不自动 commit。
3. 随后完成 M3 分支到 `main` 的 PR 合并和本地同步，再建立 M4 功能分支。
4. M4 第一个核心实验只做结构化 Queue 生产者/消费者：编码前先定义消息结构、数据所有权、阻塞时间、Queue 满策略和验收方法。

不要提前编写 ADC DMA、UART DMA、CAN、Bootloader、Watchdog 或 HardFault 代码。
