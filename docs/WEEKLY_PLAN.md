# 周计划：2026-W44

> 唯一回答：这周准备干什么？
> 周期：2026-10-26 至 2026-11-01。每周更新一次。
> 状态校准：2026-11-01；M5 基础采集链已验收并通过 PR #6 合并（`30b0072`），当前进入 `feature/m6-baseline`。M6-P1/P2 已完成基础收包和软件背压对照，M6-P3 待开始；M6 功能与文档 Git 归档尚无最新终端确认。

## Current Milestone

M6：USART2 DMA + IDLE 不定长接收；在本周末的 M5 完成与 PR 合并基础上建立 M6-P1/P2 最小链路。M6 目前独立验证 USART2，不把 M5 ADC/DMA2 新回归设为当前里程碑门槛。

## 本周核心目标

- M5：完成 ADC1_IN1 + TIM2 TRGO + DMA2 Circular 基础采样、延时压力和重启验收，并经 PR #6 合并到 main（已完成，历史证据保留）。
- M6-P1：CubeMX USART2 RX DMA1 Stream5 Channel4 Normal + IRQ 配置、生成代码 Diff 与 LF/CRLF 清理（已完成）。
- M6-P2：打通 DMA 64 B + IDLE/TC → ISR 非阻塞 Queue → UARTTask，验证短包/二进制/边界长度（已完成基础功能）。
- 修复正常 RxEvent 错记 UART Error 的统计问题；比较 Queue 深度 8/64 在持续输入下的 Drop 和 RAM（已完成）。
- 保存 M6-P2 代码、日志和测试文档，确认 Git/Build 证据后归档；准备 M6-P3 Circular DMA + 字节 Ring Buffer（待完成）。

## 本周任务

1. 已完成：M5 的已确认基础功能、压力与复位回归；M5 PR #6 合并至 `main`，Merge Commit `30b0072`，创建 `feature/m6-baseline`。
2. 已完成：M6-P1 USART2 RX DMA1 Stream5 Channel4 Normal，USART2 与 DMA1 Stream5 IRQ；EOL 处理后 Git Diff 5 文件 `78 insertions(+), 1 deletion(-)`，`git diff --check` 无输出。
3. 已完成：M6-P2 64 B Normal DMA ReceiveToIdle；指定长度 1/4/6/31/32/63/64 B 与二进制内容测试 PASS；31/63 B 为用户明确确认。
4. 已完成：错误计数从普通接收回调移除，11 次 ASCII `01 02 03 04`（各 11 B）收发通过，`Count=11 Drop=0 StartFail=0`，`TotalError=0`。
5. 已完成：13×3599 字符长 ASCII 发送记录的 8/64 格 Queue 对照。8 格 `Count=747 Drop=49`、Heap 11704 B；64 格 `Count=753 Drop=0`、Heap 7672 B；两组 `StartFail=0`、错误分类统计 0。
6. 待完成：确认本轮最终 Debug Clean Build 和 M6-P2 业务代码/文档的 Git Diff、Commit/Push、最新 clean 工作区；不要沿用 M6-P1 的五文件 Diff 当作 M6-P2 提交清单。
7. 下一阶段：M6-P3 DMA Circular + 独立 `uint8_t` Ring Buffer + 连续流字节总量/序号验收；不先宣称已实现。

## 验收标准

- M6-P2 已通过不定长接收与边界内容测试，IDLE/TC 事件符合预期；`RXE Total=0` 的负向对照证明正常数据不再误记错误。
- 8 格组发生实际 Queue 丢弃 49 次，应如实记录；64 格组本轮 Drop=0，仅限软件队列入队，不是全链路无丢字节证明。
- M6-P3 必须量化已发送、已报告接收、成功缓存与实际消费字节，测试 HT/TC/IDLE 重叠、Wrap-around、Overflow 和 10/50 KB 连续流。
- Git 归档前检查 `.ioc`/USART/DMA/IRQ/RTOS 用户代码、EOL 与暂存区，确认实际 Clean Build 和板级证据，不虚构 Commit Hash。

## 风险

- Normal DMA IDLE/TC 后 Stop/Restart 存在接收空窗；`StartFail=0` 不代表 100% 收到字节。
- 软件 Queue Full/Drop 与 UART FE/ORE 等外设错误不同；同一窗口 `TotalError=0` 仍可能发生 Queue 丢弃。
- 64 格 Queue 使 Free Heap 减少 4032 B，后续还需为 Ring Buffer、协议和设备管理预留 SRAM；只靠扩容不能解决持续生产速度大于消费速度的问题。
- `UARTFree` 测得最低 336～344 B，后续新增局部数组/日志应监视栈余量。
- CubeMX 再生成可能引起 LF/CRLF 工作区噪音；避免未审核恢复、硬重置或 `git add .`。
- M6 当前测试不包含真实 UART 硬件错误注入及恢复，也未验证端到端严格无损。

## 可选

正式归档后可用更轻量的统计输出替代每次 64 B 十六进制日志，减少同步 UART TX 对消费任务的干扰；M6-P3 优先实现可验证的 Circular DMA 指针与溢出统计，再优化吞吐。
