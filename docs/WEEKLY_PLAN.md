# 周计划：2026-W44

> 唯一回答：这周准备干什么？
> 周期：2026-10-26 至 2026-11-01。每周更新一次。
> 状态校准：2026-10-29；M5-P0 功能/文档已提交并推送，M5-P1～P5 基础采集链与板级回归通过；本轮 ADC 功能/文档仍待 Git 归档。

## Current Milestone

M5：ADC + Timer + DMA 实时数据采集。本周从 M5-P0 清理正式运行态过渡到 PA1/ADC1 + TIM2 TRGO + DMA2 Circular + FreeRTOS 的最小连续采集链，已完成规定的电压变化、延时压力和用户报告的长时间/复位回归。

## 本周核心目标

- 完成 M5-P0 任务与日志精简、板级验收及独立 Git Checkpoint（已完成）。
- 以 10 kΩ 电位器建立 TIM2 硬件触发 ADC1_IN1 的 50 Hz（设计值）采样链，DMA Half/Full 唤醒 FreeRTOS Acquisition Task（已完成）。
- 观察电压全量程变化、回调与处理计数，使用 0/50/250 Tick 延时验证 Late 对任务处理超时的响应，并恢复正式无人工延时版本（已完成）。
- 完成长时间运行与六次复位验收（用户报告通过），整理实测数据与已知边界，最后将 ADC 功能与九份文档分别提交推送（归档待完成）。

## 本周任务

1. 已完成：M4 PR #4 / #5 合并记录核对，M5 工作分支 `feature/m5-adc-timer-dma-baseline` 建立；M5-P0 功能 `942fcc3`、文档 `e7f525c` 均已推送且上次 Git 工作区确认 clean。
2. 已完成：M5-P0 精简为 HeartBeat / UART 两应用任务；30 秒 UART 882→9 行，Free Heap 3432→13504 B；NRST ×3 / Power-cycle ×3 通过。
3. 已完成：TIM2 84 MHz / PSC8399 / ARR199 / TRGO Update，ADC1_IN1（PA1）、DMA2 Stream0 Circular；10 kΩ 电位器完整接线与 Sampling Time 84 cycles 设置。
4. 已完成：新增第三应用任务 `AcquisitionTask`，使用 DMA Half/Full Callback → Thread Flags → 10 点平均电压 → UART；电位器记录 `2～3287 mV`，Error=0。
5. 已完成：A=0 Tick / B=50 Tick 时 Late=0；C=250 Tick 人为延时 Late 从 27→57→87，恢复无延时后 `Half=73/73 Full=72/72 Late=0 Error=0`。
6. 已完成：最终增量 Debug 构建成功（RAM 21448 B、Flash 35680 B）、Heap/MinHeap 12368 B，用户确认长时间运行、NRST ×3、Power-cycle ×3 通过。
7. 待完成：审查并归档本轮 ADC `.ioc` / HAL 生成源 / FreeRTOS Task / CMake 的有效代码差异；按功能、文档独立提交并推送，核对新 HEAD、工作区 clean。
8. 可选补测：Clean Build（`--clean-first`）、真实长时间运行精确时长及电压校准记录；不能预先标 PASS。

## 验收标准

- 已提供的日志证明周期采样与电位器数据变化正常，Half/Full Callback 和 Processed 计数在正式模式下匹配；`Error=0`、`Late=0`。
- 0/50/250 Tick 测试对比有可复现实测记录；250 Tick 检测到超时风险，正式固件不留人为测试延时。
- 长时间运行与 6 次复位测试由用户报告通过，记录实际证据边界，不能虚构分钟数或逐样本无丢失结论。
- 代码与 docs 的 Git 提交必须经 Diff、暂存区、EOL 审核后产生；当前不将 ADC 功能虚记为已 Commit/Push。

## 风险

- DMA Circular 继续写入内存，不因 FreeRTOS 任务阻塞而等待；Thread Flags 可能合并，同步计数与 Late=0 都不能严格证明从未覆盖样本。
- ADC 以 3300 mV 假定参考值换算，尚无万用表/校准源精度验证；输出间隔缺乏精确时间戳，不把日志推算当计量级频率精度。
- DMA 回调 ISR 不能调用阻塞 UART Logger；NVIC 优先级须满足实际 RTOS ISR API 约束。
- CubeMX 再生成可能带来大量 CRLF/LF 无关变更，需分离第三方源码噪声与有效配置，不无审查 `git add .`。
- 长时间运行时长未量化；基本验收不替代极端负载和工业可靠性专项认证。

## 可选

若归档前仍有精力，可补做一次干净 Debug 构建并测定准确 RTOS Tick 频率；在不影响主线情况下，规划未来的逐半块序号/所有权验收，不急于引入新 Task 或 Queue。
