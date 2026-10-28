# 周计划：2026-W44

> 唯一回答：这周准备干什么？
> 周期：2026-10-26 至 2026-11-01。每周更新一次。
> 状态校准：2026-10-28；M4 PR 已合并，M5-P0 代码与板级验收通过，功能/文档尚待 Git 提交；不将 ADC 开发写为已完成。

## Current Milestone

M5：ADC + Timer + DMA 实时数据采集，当前在 M5-P0 清理归档和 M5-P1 资源规划衔接阶段。M0～M4 已完成；M4 经 PR #4 / PR #5 合入 main，最终 Merge Commit `e793039`。

本周先把 M4 实验固件转换为 M5 正式运行态（只运行 HeartBeat / UART 两个应用任务），保留必要的可复用模块和历史 Git 证据，再为真实 ADC 采集做资源设计。

## 本周核心目标

- 固定 M5 清理前 Git 基线，删除不再需要的 M4 实验任务 / 对象 / 变量，保留需要复用的接口与模块。
- 验证编译、串口噪音降低、FreeRTOS Heap/Stack 变化和复位恢复能力。
- 更新 `docs/` 九份文档中受状态变化影响的内容，审核并提交 M5-P0 独立 checkpoint。
- 进入 ADC/Timer/DMA 资源核对及 Half/Full 缓冲区所有权设计；本周不把尚未实现的采集链标为完成。

## 本周任务

1. 已完成：确认 M4 合并至 main（`ccbd176`、`3a8fa13`、`e793039`）并使用 `feature/m5-adc-timer-dma-baseline` 开发。
2. 已完成：CubeMX 正式应用任务从 6 个减少至 2 个，删除 Semaphore/Mutex 演示任务和资源，清理 M4 IPC / ISR 演示启动路径，保留可复用模块与测试接口。
3. 已完成：Debug 增量构建成功，RAM 21128 B、FLASH 27916 B。
4. 已完成：清理前 30 秒日志 882 行 / Free Heap 3432 B，清理后 30 秒 9 行 / Free Heap 13504 B；应用任务数由用户实测确认为 2。
5. 已完成：本次固件 NRST ×3、完全断电重启 ×3 均由用户确认 PASS。
6. 进行中：CubeMX EOL-only 噪音整理；已将 57 个修改文件缩减至少量业务差异，最终 Git status / staging 审核仍待确认。
7. 待完成：导入更新后的 docs ZIP，形成 M5-P0 源码和文档提交并推送，核验工作区 clean。
8. 可选补测：`cmake --build --preset Debug --clean-first` 并保留输出；当前提供的是 `[2/2] Linking` 的增量构建。
9. 待开始：M5-P1 ADC 输入、Timer TRGO、DMA Stream/Channel 及采样缓冲所有权规划，不提前实现 DMA 业务驱动。

## 验收标准

- 默认应用任务恰为 HeartBeat + UART 两个，不出现 M4 演示任务日志；保留的 Ring Buffer、Debug Logger 不受本次删除影响。
- Debug 构建和 30 秒 UART / 运行时资源对比有实测依据；NRST ×3、Power-cycle ×3 通过。
- 提交的 Diff 不包含非预期 FreeRTOS 内核改动或 CubeMX 换行符噪音；`.ioc`、用户源码和 Git 记录一致。
- M5-P0 新 Commit / Push 只有在本地真实执行后才填；M5 ADC/Timer/DMA 在没有实测前不得标记完成。

## 风险

- `M4_ENABLE_SPSC_TEST` 如果因 CMake Cache 残留 ON，可能出现非预期测试任务；需要在专项构建时显式确认开关。
- TIM6 是 HAL Timebase，TIM7 仍有历史外设配置；ADC 采样触发源与 DMA Stream/Channel 必须先做资源检查。
- UART 同步日志会干扰高频采样时序；ADC DMA Half/Full 必须区分事件通知、缓冲区所有权、数据复制时限和真实丢样统计。
- CubeMX Generate Code 可能重复产生 CRLF/LF 噪音；不要不审查就 `git add .` / `git reset --hard`。
- 30 秒稳定和 6 次重启不等同于满载长期稳定、最坏响应时间或工业级可靠性认证。

## 可选

在不影响本周主线情况下，建立 M5 ADC 最小实验的硬件接线表、采样频率与 DMA 半缓冲区长度草案，并设计下一轮可量化的实时性指标。
