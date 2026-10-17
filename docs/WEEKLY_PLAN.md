
# 周计划：2026-W42

> 唯一回答：这周准备干什么？
> 周期：2026-10-12 至 2026-10-18。每周更新一次。

## Current Milestone

M4：RTOS 通信、同步和数据缓冲，正在进行。

M0～M3 已完成。

M4.1 Queue Producer / Consumer baseline 已完成。

M4.2 Task Notification / CMSIS-RTOS2 Thread Flags baseline 已完成。

M4.3 Binary Semaphore、Mutex、Stack / Heap 调优和历史联合运行基线已完成。

M4.4 Ring Buffer 基础功能已完成确定性板级验证。

当前进入 M4.5 ISR → Task 通信边界准备阶段。

本周不提前进入 M5 ADC + Timer + DMA。

## 本周核心目标

完成 M4 Ring Buffer 基础功能及真实板级验证，建立可追溯的功能 checkpoint。

完成 Ring Buffer 开发、测试和 Git 文档收尾。

在此基础上准备 ISR → Task 最小通信链路，明确 ISR-safe API、事件通知、Task 唤醒和 Ring Buffer 数据所有权。

根据实际进度推进 M4 Final Integration，不以提前关闭 M4 为目标。

## 本周任务

1. 已完成：设计独立 Ring Buffer 数据结构。
2. 已完成：建立 `Inc/ring_buffer.h`。
3. 已完成：建立 `Src/ring_buffer.c`。
4. 已完成：通过根目录 CMakeLists.txt 加入构建。
5. 已完成：实现 RingBuffer_Init。
6. 已完成：实现 RingBuffer_WriteByte。
7. 已完成：实现 RingBuffer_ReadByte。
8. 已完成：实现 RingBuffer_IsEmpty / IsFull。
9. 已完成：确定预留一个空槽的设计。
10. 已完成：明确 Full 拒绝写入策略。
11. 已完成：增加 overflow_count 溢出统计。
12. 已完成：修复开发过程中的指针和返回状态问题。
13. 已完成：建立双 Task 确定性测试。
14. 已完成：验证 Empty。
15. 已完成：验证 Full。
16. 已完成：验证 Full 后已有数据保持。
17. 已完成：验证 Write Index Wrap-around。
18. 已完成：验证 Read Index Wrap-around。
19. 已完成：验证 FIFO 数据顺序。
20. 已完成：观察 Queue、Thread Flags、Mutex、UART 继续运行。
21. 已完成：测量当前任务 Stack 和 FreeRTOS Heap。
22. 已完成：Ring Buffer 功能 Git checkpoint。
23. 已完成：`f8c3e52` 远程推送。
24. 待完成：Ring Buffer 测试与开发文档同步。
25. 待完成：形成独立 docs Git checkpoint。
26. 待完成：恢复或隔离临时复用的 Semaphore Task。
27. 待完成：重新确认原 Semaphore 功能。
28. 待完成：选择 ISR → Task 最小测试中断源。
29. 待完成：核对 CMSIS-RTOS2 ISR-safe API。
30. 待完成：建立最小 ISR → Task 通知实验。
31. 待完成：验证 Task 阻塞等待和事件唤醒。
32. 待完成：明确 Ring Buffer SPSC 数据所有权。
33. 待完成：检查 ISR / Task 并发访问要求。
34. 后续：M4 Final Integration、长期稳定性和复位恢复。

## 验收标准

- Ring Buffer 基础模块独立于 HAL / FreeRTOS。
- Ring Buffer 不使用动态内存。
- Empty / Full / Wrap-around 确定性测试全部 PASS。
- FIFO 数据顺序正确。
- Full 时拒绝写入，不破坏已有数据。
- overflow_count 在实际溢出时增加。
- Ring Buffer 功能 checkpoint 已提交并推送。
- 当前 Ring Buffer 基础功能满足进入 ISR → Task 学习阶段的条件。
- 原 Semaphore 临时测试改动需要恢复或隔离。
- ISR → Task 尚需独立真实硬件验证。
- ISR 内不得使用当前同步 LOG_xxx()。
- ISR 内不得调用阻塞 API。
- ISR / Task 共享缓冲区必须建立明确的数据所有权。
- Stack 和 FreeRTOS Heap 需要继续测量。
- M4 最终关闭前需要恢复全部正式功能并完成回归。
- M4 关闭前不开始 M5 业务实现。

## 风险

- Ring Buffer 基础功能正确不代表并发安全。
- ISR 与 Task 共享 head / tail 时需要验证原子性和内存可见性。
- volatile 不能自动替代完整同步方案。
- 覆盖旧数据会改变 tail 所有权，可能引入并发竞争。
- 当前 Ring Buffer 只支持单字节存取，未实现 DMA 数据块所有权。
- 满时拒绝写入会导致新数据丢失，必须保留溢出统计。
- Ring Buffer 容量无法解决长期生产速度高于消费速度的问题。
- 当前同步 UART Logger 会影响任务执行时序。
- 当前 Semaphore 两个 Task 的 Stack 余量需要继续关注。
- 临时复用 Semaphore Task 会使原同步实验暂时不可用。
- M4 完成前需要继续保持 CubeMX 生成代码与应用模块边界。
- Thread Flags 相同 bit 会合并，不能将单 bit 作为无损事件计数器。
- M5 DMA 事件与 Buffer Ownership 需要分开设计。

## 可选

如果 M4.5 提前完成且最终回归具备条件，可开始 M5 前置设计检查：

- ADC 通道候选。
- Timer 资源候选。
- DMA Stream / Channel 冲突检查。
- DMA Half / Full Buffer 事件设计。
- ADC 采样速度和消费能力预算。
- Buffer Ownership 草案。
- 数据丢失统计和告警需求草案。

在 M4 正式关闭前，不提交 M5 功能代码。
