# 周计划：2026-W41

> 唯一回答：这周准备干什么？
> 周期：2026-10-05 至 2026-10-11。每周更新一次。

## Current Milestone

M4：RTOS 通信、同步和数据缓冲，正在进行。

M0～M3 已完成。

M4.1 Queue Producer / Consumer baseline 已完成。

M4.2 Task Notification / CMSIS-RTOS2 Thread Flags baseline 已完成。

M4.3 Binary Semaphore、Mutex 对比、同步任务 Stack 调优和联合长期运行验证已完成。

本周下一步进入 Ring Buffer 和 ISR → Task 通信边界，不提前进入 M5 ADC + Timer + DMA 实现。

## 本周核心目标

在已经完成 Queue 数据传递和 Thread Flags 轻量事件通知基础上，继续完成 Binary Semaphore、Mutex 语义对比、基础 Ring Buffer 以及 ISR → Task 通信边界。

最终目标是关闭 M4，并形成能够支持 M5 ADC + Timer + DMA 数据采集的 RTOS 通信与数据缓冲基础。

## 本周任务

1. 已完成：M4.2 Task → Task Thread Flags 正常通知实验。
2. 已完成：`osThreadFlagsWait(..., osWaitForever)` 阻塞等待与事件唤醒。
3. 已完成：相同 Flag bit 重复 Set 的事件合并行为。
4. 已完成：33 Sets → 11 Receives Burst 实验。
5. 已完成：Thread Flags 新增 Task Stack 和 FreeRTOS Heap 测量。
6. 已完成：M4.2 Final Baseline 稳定性验证。
7. 已完成：M4.3 Binary Semaphore Task → Task 同步实验。
8. 已完成：Binary Semaphore 与 Mutex 职责对比。
9. 已完成：`SharedDataMutex` 双 Task 共享数据互斥实验。
10. 已完成：使用 `shared_op_seq` 验证真实临界区执行顺序。
11. 已完成：Queue + Thread Flags + Semaphore + Mutex 联合长期运行。
12. 已完成：发现 Semaphore / Mutex Task Stack 余量不足并完成调优。
13. 已完成：Stack 调整后重新验证 High Water Mark 和 FreeRTOS Heap。
14. 下一步：实现独立 Ring Buffer。
15. 验证 Ring Buffer：
    - Empty。
    - Full。
    - Wrap-around。
    - Read / Write Index。
    - 数据顺序。
16. Ring Buffer 通过后建立最小 ISR → Task 通信边界实验。
17. M4 最终关闭前核对 `freertos.c` 与 `m4_ipc` 模块边界。
18. 完成 M4 Final Baseline 稳定性、文档和 Git 收尾。

## 验收标准

- M4.1 Queue 已有独立功能和文档 checkpoint。
- M4.2 Thread Flags 已有真实板级证据。
- Queue 与 Thread Flags 职责差异已明确。
- 相同 Flag bit 事件合并语义已验证。
- Binary Semaphore Give / Take、阻塞等待和唤醒已完成真实板级验证。
- Binary Semaphore 与 Mutex 在事件同步和资源所有权上的差异已通过实机实验建立。
- Mutex 共享数据 Read-Modify-Write 保护已通过 `shared_op_seq` 验证。
- Queue + Thread Flags + Semaphore + Mutex 已完成联合长期运行。
- 新增同步 Task Stack 已完成 High Water Mark 测量与调优。
- FreeRTOS Heap 调整后保持稳定。
- Ring Buffer 仍需完成 Empty、Full 和 Wrap-around。
- ISR → Task 实验仍需明确安全、非阻塞的通信边界。
- 当前同步 `LOG_xxx()` 不进入 ISR。
- 临时压力参数全部必须在 Final Baseline 前恢复。
- M4 正式关闭前不开始 M5 业务实现。

## 风险

- Thread Flags 相同 bit 会合并，不能把单一 Flag 当作无损事件计数器。
- Binary Semaphore 同样具有容量 / 状态语义，不能未经验证就假设能保存任意数量事件。
- Binary Semaphore 不应被当作 Mutex 替代品。
- Mutex 的资源所有权和优先级继承语义需要与普通同步机制保持区分。
- 同步 UART Logger 会改变任务执行时序，日志顺序不能直接视为内核 Trace。
- 异常日志路径可能比正常路径使用更多 Task Stack。
- Ring Buffer 如果进入 ISR / Task 并发访问，需要明确单生产者 / 单消费者假设、读写所有权和临界区策略。
- M5 DMA 事件与 Buffer 数据所有权必须分开设计，不能只依赖一个 DATA_READY bit。
- Queue / Ring Buffer 只能吸收有限突发，不能解决长期 Producer Throughput 大于 Consumer Throughput 的问题。

## 可选

如果 Binary Semaphore、Ring Buffer 和 ISR → Task 边界均提前完成，并且 M4 Final Baseline 已通过稳定性验证，可以开始做 M5 前置设计检查：

- ADC 通道候选。
- Timer 资源候选。
- DMA Stream / Channel 冲突检查。
- Half / Full Buffer 事件设计。
- 数据 Buffer Ownership 草案。

在 M4 正式关闭前，不提交 M5 功能代码。
