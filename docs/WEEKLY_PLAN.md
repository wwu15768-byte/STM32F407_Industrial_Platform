# 周计划：2026-W43

> 唯一回答：这周准备干什么？
> 周期：2026-10-19 至 2026-10-25。每周更新一次。
> 状态校准：2026-10-23；完成项以用户提交和板级观察为准，未记录的测试不标为通过。

## Current Milestone

M4：RTOS 通信、同步和数据缓冲，当前尚未关闭。

M0～M3 已完成；M4.1 Queue、M4.2 Thread Flags、M4.3 Binary Semaphore / Mutex、M4.4 Ring Buffer Basic 已有历史板级验证。

M4.5 TIM7 ISR → Task baseline 已完成首次板级及用户确认的持续运行验证，最新功能 checkpoint 为 `adf5d5e`。

当前推进：M4.5 文档收尾、M4 Final Integration 前置检查；暂不进入 M5 ADC + Timer + DMA。

## 本周核心目标

- 整理 M4.5 的真实板级日志、构建资源与持续运行验证结果。
- 以已有 `docs/` 原格式同步项目状态、决策、错误、测试、知识缺口及交接信息。
- 建立独立文档 Git checkpoint，不与 M4.5 功能提交混合。
- 着手 M4 Final Integration 的任务与模块边界检查；未验证的最终系统级项目保持待完成。

## 本周任务

1. 已完成：TIM7 时钟与初始化配置核对，PSC=8399、ARR=999，100 ms 更新周期。
2. 已完成：HAL TIM6 Timebase 与 TIM7 回调来源区分，TIM7 不额外执行 `HAL_IncTick()`。
3. 已完成：NVIC Grouping 4-bit、TIM7 IRQ 优先级 5 与 FreeRTOS ISR-safe API 阈值 5 核对。
4. 已完成：建立独立 `m4_isr` Worker / ISR 通知模块，初始化返回模块专属枚举状态码。
5. 已完成：CMake Debug Clean Build 和 `git diff --check`。
6. 已完成：板级日志 `irq=30/40/50/60` 对应 `received=30/40/50/60`、`errors=0`。
7. 已完成：原 Queue、Task Flags、Semaphore、Mutex、UART 的当前短时联合回归。
8. 已完成：SYS 资源采样，Worker Stack 最小剩余 520 B，Heap/MinHeap 3432 B。
9. 已完成：用户确认持续运行测试均通过；末尾计数、准确时长未附于文档输入。
10. 已完成：M4.5 功能 Git checkpoint `adf5d5e`，本地与 origin 同步，工作树 clean（提交后用户确认）。
11. 进行中：九份 Markdown 文档同步与人工审查。
12. 待完成：提交并推送独立的 docs checkpoint。
13. 待完成：按最新源文件核对 Ring Buffer 的实际元素类型、字节容量及 `WriteByte`/`ReadByte` 命名一致性。
14. 待完成：整理/隔离 Ring Buffer 确定性测试入口，确认正式 Semaphore 同步和 M4ISR 任务长期共存。
15. 待完成：M4 Final Integration 的完整功能回归与资源收尾。
16. 待完成：M4 最终长期运行、NRST ×3、Power-cycle ×3。
17. 待完成：M4 最终 Git/PR 收尾，再考虑进入 M5。

## 验收标准

- 当前提交 `adf5d5e` 可作为已验证 M4.5 功能基线，且不误标为 M4 Final Baseline。
- TIM7 使用独立中断，ISR 不记录同步 UART 日志、不使用阻塞 API；Task 使用 Thread Flags 阻塞等待。
- `irq` / `received` 的低负载采样对应、`errors=0`；保留重复事件合并语义说明。
- Stack / Heap 记录来自真实运行日志，不使用 ELF 静态 RAM 代替动态资源指标。
- CubeMX 重新生成的 CRLF/LF 噪音已定向处理，不将中间件无关格式化混入提交。
- 九份文档术语、日期、代码 checkpoint、当前进度和未完成事项一致。
- M4 Final Integration 的未执行测试不得写成 PASS。
- 完整回归与 NRST / Power-cycle 验证后，才能评估 M4 正式关闭。

## 风险

- M4ISR 后当前 Free Heap 3432 B；继续增加 Task/Queue 前必须预算动态分配空间。
- Worker 剩余栈 520 B；Semaphore Producer/Consumer 288 B，后续异常路径和日志仍需监控。
- Thread Flags 不累计同一 bit 的所有事件，低负载 `irq=received` 不代表零丢失保证。
- 已验证的 Ring Buffer Basic 不等于 ISR/Task SPSC 并发安全。
- 既有 docs 对 Ring Buffer 字节宽度记载与此前源码快照不一致，最终集成前必须按最新源码确认。
- CubeMX Generate Code 会重新引入 CRLF 工作区噪音，须复用备份、定向规范化与审核 Diff 的流程。
- Sync UART Logger 会改变 Task 时序，不能用于 ISR。

## 可选

在不开始 M5 业务代码的前提下，可准备：ADC/Timer/DMA 资源冲突清单、DMA Half/Full 事件设计、数据缓存所有权与吞吐量预算。优先级低于 M4 Final Integration。
