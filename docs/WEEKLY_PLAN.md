# 周计划：2026-W44

> 唯一回答：这周准备干什么？
> 周期：2026-10-26 至 2026-11-01。每周更新一次。
> 状态校准：2026-10-27；只将实际板级/提交证据标记完成，PR 与复位验收未确认前不写为完成。

## Current Milestone

M4：RTOS 通信、同步和数据缓冲，当前进入最终合并/关闭准备，**尚未正式关闭**。M0～M3 已完成。

M4.1 Queue、M4.2 Task Flags、M4.3 Semaphore/Mutex、M4.4 Ring Buffer 基础功能、M4.5 TIM7 ISR → Task 均有板级基线；2026-10-27 新增 **Task ↔ Task SPSC 并发 50,000 元素 PASS**，功能 checkpoint `d93665e` 在独立分支。

本周不进入 M5 ADC + Timer + DMA 业务实现，先完成当前 SPSC 分支合并和正式 M4 基线验收。

## 本周核心目标

- 以最新 SPSC 并发测试补齐 Ring Buffer 在 M4 关闭前的指定验证缺口。
- 记录 Full/Empty 背压、连续数据序号、C11 原子索引、Stack/Heap 与现有模块联合运行证据。
- 确认 `M4_ENABLE_SPSC_TEST=OFF` 的正式固件可重新 clean build 与烧录运行。
- 在证据齐全后通过 PR 将 SPSC 并入 M4，完成文档和最终回归，再决定是否关闭 M4。

## 本周任务

1. 已完成：基于干净仓库创建独立 `feature/m4-spsc-concurrency-verification` 分支。
2. 已完成：`ring_buffer.h/.c` 的 C11 `_Atomic` head/tail 和 acquire/release SPSC 内存序调整，元素为 `uint32_t`，有效容量 7 个元素。
3. 已完成：新增两独立 SPSC Task 以及 `M4_ENABLE_SPSC_TEST` 可选 CMake 开关（默认 OFF），保持 Semaphore Task 职责分离。
4. 已完成：板级 `PASS sent=50000 recv=50000 full=7064 empty=7419`、`overflow=7064 elapsedTicks=7501`。
5. 已完成：同窗口验证 M4Q、M4F、Semaphore、Mutex、TIM7 ISR 正常，采样 Heap/MinHeap 3432 B、SPSC P/C Stack 剩余 876/444 B。
6. 已完成：功能独立提交 `d93665e` 并推送 origin；用户确认工作区干净。
7. 待提供证据：SPSC 版本 NRST ×3、Power-cycle ×3 与 SPSC PASS 后的其他模块持续运行终点记录。
8. 待提供证据：显式 `M4_ENABLE_SPSC_TEST=OFF` 后 clean build、烧录、无 `[SPSC] START` 的正式回归。
9. 待完成：SPSC → M4 Pull Request 审查、合并、更新本地 M4 分支与合并后板级检查。
10. 进行中：九份 Markdown 的 M4 Final Integration / SPSC 记录更新；导入仓库后核对 Diff 和提交独立 docs checkpoint。
11. 待完成：确认 M4 最终验收边界及 PR M4 → main；通过后再计划 M5 分支。

## 验收标准

- Task ↔ Task SPSC 实际发送/接收同为 50,000，且顺序无错、Full 和 Empty 被实测触发；Full 重试不误写为最终数据丢失。
- `SpscPFree=876 B`、`SpscCFree=444 B` 和 Heap 3432 B 来自板级采样，不将静态 RAM 与 FreeRTOS Heap 混淆。
- SPSC 只以一个 Producer 写 head、一个 Consumer 写 tail；不能推广到 ISR ↔ Task / DMA / MPSC。
- 原 M4 Queue、Flags、Semaphore、Mutex、TIM7 ISR 运行不受本轮测试影响（以日志窗口为准）。
- 正式固件显式关闭 SPSC 测试后重新生成、烧录并通过回归，缓存选项不残留 ON。
- 最终复位/上电重启和合并后验收均有独立记录，Git HEAD / Branch 与文档当前状态一致。
- 未完成的 PR、NRST 和 Power-cycle 不得写成已完成。

## 风险

- 临时测试与正式固件配置不同；构建 Cache 保留 `ON` 会导致测试任务意外存在。
- SPSC 当前 Consumer 剩余 Stack 444 B，Semaphore 任务剩余 Stack 288 B，M5 增加业务前必须重新测量。
- `overflow_count` 当前统计失败写入尝试，并非业务丢样计数；需要另设真实丢样指标。
- C11 原子 SPSC 只覆盖一个 Producer/一个 Consumer，ISR/DMA 生产侧不可直接照搬结论。
- CubeMX 再生成易引入 CRLF/LF 噪音，保留备份和定向处理流程。
- 单次 `7501 ticks` 的测试不代替长期满载、复位与工业级最坏延迟验收。

## 可选

M4 经验证合并后再准备 M5 ADC/Timer/DMA 资源冲突清单、Half/Full 数据所有权图和吞吐预算；不提前实现 M5 业务逻辑。
