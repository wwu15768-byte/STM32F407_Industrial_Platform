# 周计划：2026-W40

> 唯一回答：这周准备干什么？
> 周期：2026-09-28 至 2026-10-04。每周更新一次。

## Current Milestone

M4：RTOS 通信、同步和数据缓冲，正在进行。

M0～M3 已完成。

M4.1 Queue Producer / Consumer baseline 已完成并形成 `b78282c`。本周继续推进 M4，不进入 ADC + Timer + DMA、UART DMA、RS485、Modbus、CAN、Bootloader 或 Watchdog。

## 本周核心目标

在已经完成的 Queue 数据传递基础上继续建立 Task Notification 和 Binary Semaphore 的最小验证链路，并逐步形成 Queue、Notification、Semaphore、Mutex 的职责边界。

Ring Buffer 只有在前述 RTOS 通信与同步机制已经能够解释并验证后再开始。

## 本周任务

1. 已完成：从 M3 稳定 `main` 建立 `feature/m4-rtos-communication-baseline`。
2. 已完成：建立结构化 Queue Producer / Consumer 实验。
3. 已完成：验证结构体按值传递、Consumer 阻塞等待、Queue Full、Backpressure、Drop 和 Sequence Gap。
4. 已完成：重新测量 M4 Task Stack 和 FreeRTOS Heap，并根据压力结果将 Producer Stack 从 512 B 调整为 1024 B。
5. 下一步：建立 Task Notification / CMSIS-RTOS2 Thread Flags 最小 Task → Task 通知实验。
6. 比较 Queue 的“传数据”语义与 Task Notification 的“通知事件”语义。
7. Notification 验证完成后建立 Binary Semaphore 实验，并与已有 Debug Logger Mutex 对比。
8. 仅在上述机制已经通过验证后开始基础 Ring Buffer。

## 验收标准

- M4.1 Queue checkpoint 已有独立功能 commit 和真实板级证据。
- 能解释 Queue 数据所有权和按值复制行为。
- 能解释 Queue 满、背压、Drop 和 Sequence Gap 的因果关系。
- Task Notification 实验能稳定完成通知、阻塞等待和唤醒。
- 能说明 Queue 与 Task Notification 的适用场景差异。
- Binary Semaphore 实验能够完成明确的同步链路。
- 能说明 Binary Semaphore 与 Mutex 在事件同步和资源所有权上的差异。
- 新增重要 Task 或 RTOS Object 后重新检查 Task Stack 和 FreeRTOS Heap。
- 临时压力测试参数不能进入 Final Baseline。
- 不提前实现 M5 或后续里程碑代码。

## 风险

- 同步 UART Logger 会改变任务执行时序，不能把日志时序直接等同于 RTOS 内核精确调度时刻。
- 异常日志路径可能比正常路径使用更多 Task Stack，必须继续观察 Stack Watermark。
- Notification 不应被当作结构化数据 Queue 的直接替代品。
- Binary Semaphore 不应被当作 Mutex 使用。
- Queue / Buffer 只能吸收有限突发，不能解决长期吞吐失衡。
- Ring Buffer 如果与 ISR / Task 并发访问结合，需要单独明确读写所有权和临界区策略。

## 可选

如果 Task Notification 和 Binary Semaphore 均提前完成且能够解释，再开始 Ring Buffer 的纯数据结构实现和满 / 空 / 回绕边界测试。

本周不要求提前进入 M5。
