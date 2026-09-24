# 周计划：2026-W39

> 唯一回答：这周准备干什么？
> 周期：2026-09-21 至 2026-09-27。每周更新一次。

## Current Milestone

M2：FreeRTOS 最小系统与调度基础，完成收尾。

M3：线程安全、可裁剪的调试基础设施，准备开始。

## 本周核心目标

完成 M2 剩余的调度、运行时资源与稳定性验证，形成经过真实硬件验证的 FreeRTOS Final Baseline；完成文档和 Git 收尾后进入 M3。

## 本周任务

验证不同优先级任务之间的抢占行为。

测量 HeartbeatTask/UARTTask 的实际 Stack High Water Mark。

测量 FreeRTOS Current Free Heap 和 Minimum Ever Free Heap。

验证两个同优先级、持续 Ready Task 的时间片调度行为。

将实验 Busy Task 恢复为正常周期任务 baseline。

完成 M2 Final Baseline NRST 和完全断电重新上电稳定性验证。

清理 M2 Git 提交边界并建立最终功能 checkpoint。

同步 TEST_RECORD、PROJECT_STATUS、DAILY_LOG、KNOWLEDGE_GAPS、ROADMAP、README 和 SESSION_HANDOFF。

创建 M2 文档关闭 checkpoint 并推送远程。

如果 M2 完整关闭且本周仍有开发时间，只开始 M3 最小日志基础设计，不进入正式业务模块。

## 验收标准

不同优先级抢占实验通过。

同优先级时间片调度实验通过。

HeartBeatTask 与 UARTTask Stack 余量具有真实运行时测量数据。

FreeRTOS Heap 当前与历史最低剩余量具有真实运行时测量数据。

M2 Final Baseline 使用正常周期任务结构，不保留 CPU 满载 Busy Task 实验代码。

M2 Final Baseline NRST ×3 通过。

M2 Final Baseline完全断电重新上电 ×3 通过。

最终功能代码形成独立 Git checkpoint。

文档与真实代码、测试和 Git 状态一致。

feature/m2-freertos-baseline 最终同步到远程且工作区 clean。

ROADMAP 中 M2 标记为完成，M3 标记为当前。

## 风险

调度实验代码不能长期残留在 Final Baseline 中。

Stack/Heap 的当前测量值只适用于当前执行路径，新增功能后必须重新测量。

osThreadGetStackSpace() 反映历史最小剩余 Stack，不能简单理解为瞬时空闲 Stack。

多任务直接调用同一个 UART 输出接口在后续会产生共享资源竞争问题，M3 必须建立线程安全日志策略。

CubeMX 或 Windows 编辑器可能重新保存大量文件并改变 CRLF/LF；提交前必须区分真实代码差异与 EOL-only 噪音。

M2 完成后不能为了赶进度直接跳到 ADC、DMA、RS485 或 Modbus，应先补齐调试和并发基础设施。

## 可选任务

完成 M2 后，为 M3 设计一个最小日志接口草案，包括日志等级、统一入口和线程安全目标，但暂不进行大规模重构。