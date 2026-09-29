# 周计划：2026-W40

> 唯一回答：这周准备干什么？
> 周期：2026-09-28 至 2026-10-04。每周更新一次。

## Current Milestone

M4：RTOS 通信、同步和数据缓冲，准备正式开始。

M0～M3 已完成。本周不进入 ADC DMA、UART DMA、RS485、Modbus、CAN、Bootloader 或 Watchdog。

## 本周核心目标

在稳定的 M3 调试基线上完成 M4 启动准备，并建立第一个可解释、可验证的结构化 Queue 生产者/消费者实验；重点理解“为什么传数据、谁拥有数据、Queue 满时怎样处理”，而不是一次实现全部 M4 内容。

## 本周任务

1. 检查本次路线调整 diff，确认 ROADMAP、README、PROJECT_STATUS 和相关文档一致；由用户决定是否 commit，并完成 M3 到 `main` 的 Git 收尾后再建立 M4 功能分支。
2. 在编码前画出最小 Queue 数据流，定义消息结构、生产者、消费者、数据所有权、阻塞时间和 Queue 满策略。
3. 实现并验证两个或以上 Task 的结构化 Queue 传递，记录正常、Queue 满和 Consumer 变慢时的行为。

## 验收标准

- M0～M3 状态保持完成，M4 保持当前；没有提前实现未来里程碑代码。
- M3 分支与 `main` 的关系清楚，提交前检查 `git status`、`git diff --stat`、`git diff --check` 和完整 diff。
- 能用自己的话解释 Queue 与共享全局变量的数据所有权差异。
- 两个或以上 Task 能通过 Queue 正确传递结构化数据。
- Queue 满策略有明确选择，并有实际运行证据，不把丢数据或永久阻塞当作“正常”。
- 测试结束后更新 TEST_RECORD、PROJECT_STATUS、DAILY_LOG 和必要的 KNOWLEDGE_GAPS。

## 风险

- 不应在 M4 第一个实验中同时加入 ADC、DMA 或协议解析，否则难以定位并发问题。
- Queue Item 过大或 Queue Length 过长会直接消耗 FreeRTOS Heap，创建后必须重新测量 Heap。
- 生产者传递指针时会引入对象生命周期和所有权风险；第一个实验优先使用小型结构体按值传递。
- 日志模块的 Mutex 已验证资源保护，但不能把 Mutex 当作数据传递机制。
- 当前 M3 尚未合并到 `main`，开始 M4 功能编码前必须先建立干净、可追踪的分支基线。

## 可选挑战

仅在 Queue 实验已经通过并能完整解释后，设计 Task Notification 的最小 Task/Task 通知实验；本周不要求完成全部 M4。
