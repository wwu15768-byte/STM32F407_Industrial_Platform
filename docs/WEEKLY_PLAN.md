# 周计划：2026-W47

> 唯一回答：这周准备干什么？
> 周期：2026-11-16 至 2026-11-22。每周更新一次。
> 状态校准：2026-11-21，M6-P4 已测恢复分支及 771 B PRBS32 回归同步；DMA 真实故障专项暂缓，P4 最终构建和 Git 收尾尚未确认。

## Current Milestone

M6：UART RX DMA + IDLE 不定长接收，P3 已验收；P4 已完成部分错误分类、恢复控制与 FE 恢复/数据回归，进入正式配置与可追溯交付收尾。M6 不标记为整体完成。

## 本周核心目标

- 归档 T08-A/B、T09 ORE、T10-A NE 软件模拟以及 FE 恢复/数据回归的已测证据。
- 保留 771 B PRBS32 正常回归与资源快照，区分故障阶段和无错数据阶段。
- 确认正式测试宏配置、Clean Build、烧录、最终 PRBS 回归及 Git 归档所需证据。

## 本周任务

1. 已完成：T08-A 两次失败后成功、T08-B 重试耗尽的状态分支验证。
2. 已完成：T09 ORE 分类/恢复启动及 T10-A NE 软件模拟分类/恢复启动；真实 NE 硬件注入未开展。
3. 已完成（用户确认）：FE 故障恢复及后续已知序列回归；需保留分阶段证据，不混作单次快照。
4. 已完成：771 B PRBS32 正常数据回归，四项字节总量均为 771，`Drop=Pending=Mismatch=StatsErrors=0`；多轮快照稳定。
5. 当前文档同步：更新 `TEST_RECORD/PROJECT_STATUS/ROADMAP/SESSION_HANDOFF/DECISIONS/KNOWLEDGE_GAPS/DAILY_LOG` 的阶段口径。
6. 待完成：确认注入宏全部关闭；正式 Clean Build、烧录并执行独立 771 B 回归；获取 P4 最新 `git status`、提交/推送记录。

## 验收标准

- UART 错误控制路径与实际数据内容校验分别有直接证据；NE 软件模拟与真实故障、DMA 回调模拟与真实 DMA 故障不能混写。
- 正式构建/烧录后有独立正常接收证据：`Observed=Writed=Consumed=Checked=771`，`Drop=Pending=StatsErrors=Mismatch=0`。
- 文档与代码 Git Diff 经审核，无行尾噪音和未说明的测试开关；提交/推送以实际命令输出为准。

## 风险

- DMA 硬件错误/异步 Abort 竞态按本轮安排暂缓，不能写作通过；最坏响应时限和真实 NE 硬件故障亦未验证。
- 正常 771 B 快照 `UReady=DReady=DMAR=DMA_EN=0` 的诊断语义尚需核对源码；不以未解释的快照推断真实 DMA 停机或健康。
- 未提供 P4 当前代码、最终 Clean Build 和 Git 状态，因此不能确认已关闭全部注入模式或取得最终稳定固件。

## 可选

在不影响正式收尾的前提下，整理恢复状态图和各统计字段的采样时点，为后续 DMA 专项及 M7 RS485 复用。
