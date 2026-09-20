# 会话交接

## 当前 Milestone

M2：FreeRTOS 最小系统与调度基础，正在进行。

M1 已完成并冻结为裸机 Known Good Baseline。

## 当前 Branch

`feature/m2-freertos-baseline`，已关联远程 `origin/feature/m2-freertos-baseline`。

当前 HEAD：

`a8e588c`：`feat: establish FreeRTOS dual-task baseline`

本地分支与远程 tracking branch 已同步，工作区干净。

## 已经完成并验证的内容

- M0 已完成；`main` 与 `origin/main` 指向 `4e92819`。
- M1 已完成；最终 M1 checkpoint 为 `0b6c86f`：`docs: close M1 and prepare M2 handoff`。
- STM32F407VGT6 裸机基线已经完成构建、烧录、PB2 heartbeat、USART2 TX、NRST 和断电重启验证。
- 已创建 `feature/m2-freertos-baseline` 并从完整 M1 Known Good Baseline 开始 FreeRTOS 集成。
- HAL Timebase 已从 SysTick 迁移到 TIM6。
- TIM6 当前提供 HAL 1 ms Tick。
- SysTick 当前用于 FreeRTOS 1000 Hz Kernel Tick。
- 已启用 FreeRTOS，接口采用 CMSIS-RTOS2。
- 已加入 FreeRTOS Kernel、CMSIS-RTOS2 wrapper、ARM Cortex-M4F GCC port 和 `heap_4`。
- 已生成并使用 `FreeRTOSConfig.h`。
- `HeartbeatTask` 已完成实际运行验证，使用 `osPriorityNormal` 和 `osDelay(500U)`。
- 单任务版本已通过构建、烧录、NRST ×3 和断电重新上电 ×3。
- 单任务稳定 checkpoint 为 `4d62a23`：`feat: establish FreeRTOS single-task baseline`。
- `UARTTask` 已完成实际运行验证，使用 `osPriorityNormal` 和 `osDelay(1000U)`。
- `HeartbeatTask` 和 `UARTTask` 已在真实目标板上同时稳定运行。
- 双任务版本已通过 NRST ×3 和断电重新上电 ×3。
- 双任务稳定 checkpoint 为 `a8e588c`：`feat: establish FreeRTOS dual-task baseline`。
- `feature/m2-freertos-baseline` 已推送远程并建立 upstream。

## 当前正在进行的任务

继续理解并验证 FreeRTOS 调度机制。

当前已经证明单用户任务和两个同优先级用户任务能够正常运行，下一步准备通过最小实验验证不同优先级任务之间的抢占关系，并开始观察 Task Stack 和 FreeRTOS heap 的运行时使用情况。

## 尚未完成的内容

- 尚未通过专门实验验证不同优先级任务之间的抢占行为。
- 尚未精确验证同优先级任务时间片轮转的具体行为。
- 尚未测量 HeartbeatTask 和 UARTTask 的实际栈余量。
- 尚未测量 FreeRTOS heap 当前剩余量和最小剩余量。
- 尚未进入 Queue、Task Notification、Mutex 等任务间通信与同步。
- USART2 RX 尚未进行真实板级验证。
- 尚未进入 ADC、DMA、RS485 或 Modbus 功能阶段。

## 当前阻塞

无。

## 已确认的重要技术决策

- 固件外设层继续统一使用 STM32 HAL。
- 项目实际目标 MCU 为 STM32F407VGT6、LQFP-100。
- FreeRTOS 接口采用 CMSIS-RTOS2。
- HAL Timebase 使用 TIM6，SysTick 留给 FreeRTOS Kernel Tick。
- M2 采用逐步验证方式：先建立单任务 baseline，再建立双任务 baseline，不一次引入 Queue、Mutex、DMA 或其他复杂机制。
- 当前 HeartbeatTask 和 UARTTask 均使用 `osPriorityNormal`，先验证同优先级任务稳定共存，再进行优先级实验。
- FreeRTOS heap 与链接脚本中的 C heap 分开理解和记录。
- 每形成一个经过真实硬件验证的独立状态，就建立有意义的 Git checkpoint。
- build/、.elf、.hex、.bin 等可重新生成的构建产物继续不进入 Git。

## 最近一次有效测试

TEST-20260920-01：FreeRTOS 单任务调度基线验证通过。

TEST-20260920-02：FreeRTOS 双用户任务调度基线验证通过；HeartbeatTask 与 UARTTask 均能够稳定运行，NRST ×3 和完全断电重新上电 ×3 均通过。

## 本次交接基于的 Commit

`a8e588c`：feat: establish FreeRTOS dual-task baseline

## Working Tree 状态

当前 `feature/m2-freertos-baseline` 与 `origin/feature/m2-freertos-baseline` 已同步。

最近确认状态：

`nothing to commit, working tree clean`

进入下一次开发前仍应重新执行 `git status --short --branch`、`git log --oneline --decorate -5` 和 `git branch -vv` 核对真实状态。

## 下一步最合理的一个任务

保持当前双任务稳定基线，不进入 ADC、DMA 或正式通信模块。

先设计一个最小 FreeRTOS 优先级实验，验证高优先级 Ready Task 对低优先级 Running Task 的抢占行为；实验完成后再学习并测量 Task Stack High Water Mark 和 FreeRTOS heap 运行时余量。