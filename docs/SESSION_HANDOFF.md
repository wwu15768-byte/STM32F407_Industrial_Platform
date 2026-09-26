# 会话交接

## 当前 Milestone

M2：FreeRTOS 最小系统与调度基础，已完成。

下一阶段为 M3：线程安全、可裁剪的调试基础设施。

M1 已完成并冻结为裸机 Known Good Baseline。

## 当前 Branch

feature/m2-freertos-baseline，已关联远程 origin/feature/m2-freertos-baseline。

当前 M2 最终功能 checkpoint：

9c7333b：feat: finalize FreeRTOS scheduling baseline

当前正在进行 M2 最终文档同步；文档关闭 checkpoint 完成后统一 push。

## 已经完成并验证的内容

M0 已完成；main 与 origin/main 保持原稳定基线。

M1 已完成；最终 M1 checkpoint 为 0b6c86f：docs: close M1 and prepare M2 handoff。

STM32F407VGT6 裸机基线已经完成构建、烧录、PB2 heartbeat、USART2 TX、NRST 和断电重启验证。

M2 已从完整 M1 Known Good Baseline 开始并完成 FreeRTOS 集成。

HAL Timebase 已从 SysTick 迁移到 TIM6。

TIM6 当前提供 HAL 1 ms Tick。

SysTick 当前用于 FreeRTOS 1000 Hz Kernel Tick。

FreeRTOS 接口采用 CMSIS-RTOS2。

已加入 FreeRTOS Kernel、CMSIS-RTOS2 wrapper、ARM Cortex-M4F GCC port、heap_4 和 FreeRTOSConfig.h。

单用户任务调度已经通过真实硬件验证。

双用户周期任务稳定共存已经通过真实硬件验证。

不同优先级任务抢占行为已经通过专门实验验证。

configUSE_PREEMPTION = 1 已确认。

configUSE_TIME_SLICING = 1 最终有效配置已确认。

两个相同优先级且持续 Ready 的 Busy Task 已完成时间片调度行为验证。

Task Stack High Water Mark 已通过 osThreadGetStackSpace() 实际观测。

UARTTask 初始 512 B Stack 历史最小剩余仅 64 B，因此已经调整至 1024 B。

FreeRTOS Current Free Heap 和 Minimum Ever Free Heap 已通过运行时接口实际观测。

当前最终任务结构已恢复为正常 HeartBeatTask + UARTTask 周期 baseline。

HeartBeatTask：

Priority：osPriorityNormal

Stack：512 B

osDelay(500U)

历史最小剩余 Stack：约 384 B

UARTTask：

Priority：osPriorityNormal

Stack：1024 B

osDelay(1000U)

历史最小剩余 Stack：约 560 B

FreeRTOS Heap：

Total：15360 B

Current Free：13592 B

Minimum Ever Free：13592 B

M2 Final Baseline 已通过 NRST ×3。

M2 Final Baseline 已通过完全断电重新上电 ×3。

未观察到 HardFault、Hang 或 Heap 持续下降。

M2 最终功能 checkpoint 为 9c7333b：feat: finalize FreeRTOS scheduling baseline。

已识别并清理 CubeMX/编辑器导致的大量 CRLF/LF EOL-only 工作区噪音，没有将无关 FreeRTOS Middleware 格式变化写入功能提交。

## 当前正在进行的任务

完成 M2 最终文档同步和 Git 收尾。

功能代码和板级验证已经完成；当前只需要完成 docs/README 同步，创建 docs: close M2 and prepare M3 handoff checkpoint，并统一 push 到远程。

## 尚未完成的内容

M2 最终文档关闭 checkpoint 尚未创建。

M2 最终本地提交尚未统一 push 到远程。

尚未进入 M3 线程安全日志与调试基础设施。

尚未进入 Queue、Task Notification、Mutex 等任务间通信与同步阶段。

USART2 RX 尚未进行真实板级验证。

尚未进入 ADC、DMA、RS485 或 Modbus 功能阶段。

## 当前阻塞

无。

## 已确认的重要技术决策

固件外设层继续统一使用 STM32 HAL。

项目实际目标 MCU 为 STM32F407VGT6、LQFP-100。

FreeRTOS 应用层接口采用 CMSIS-RTOS2。

HAL Timebase 使用 TIM6，SysTick 留给 FreeRTOS Kernel Tick。

FreeRTOS 当前使用抢占式调度，configUSE_PREEMPTION = 1。

当前 FreeRTOS 时间片调度最终有效配置为 configUSE_TIME_SLICING = 1。

M2 采用逐步验证方式建立单任务、双任务、优先级、时间片和运行时资源 baseline。

UARTTask Stack 采用实际 High Water Mark 测量驱动调整，不根据经验值盲目缩减。

M2 最终 baseline 中 HeartBeatTask 和 UARTTask 均使用 osPriorityNormal 并主动阻塞，不保留 Busy Task CPU 满载实验结构。

Stack/Heap 运行时监测能力保留在当前 baseline 中，后续新增重要功能后继续重新测量。

FreeRTOS heap 与链接脚本中的传统 C heap 分开理解和记录。

每形成一个经过真实硬件验证的独立状态，就建立有意义的 Git checkpoint。

build/、.elf、.hex、.bin 等可重新生成的构建产物继续不进入 Git。

提交前不仅检查 git diff --check，还要检查真实文件边界，避免 CRLF/LF 等格式噪音污染 Git 历史。

## 最近一次有效测试

TEST-20260924-01：FreeRTOS 同优先级时间片调度验证通过；两个相同优先级、持续 Ready 的 Busy Task 均持续获得 CPU 执行机会，NRST ×3 通过。

TEST-20260924-02：M2 FreeRTOS 最终基线稳定性验证通过；HeartBeatTask、UARTTask、Stack/Heap 监测运行正常，NRST ×3 和完全断电重新上电 ×3 均通过。

## 本次交接基于的 Commit

9c7333b：feat: finalize FreeRTOS scheduling baseline

Working Tree 状态

当前正在修改 M2 最终状态文档。

文档提交完成后，应执行：

git status --short --branch

git log --oneline --decorate -8

git branch -vv

确认工作区 clean，并确认 feature/m2-freertos-baseline 与 origin/feature/m2-freertos-baseline 在最终 push 后同步。

## 下一步最合理的一个任务

不要进入 ADC、DMA、RS485 或 Modbus。

先完成当前文档 checkpoint 和远程同步，正式关闭 M2。

之后进入 M3，从最小线程安全日志基础开始：整理 UART 日志接口、日志等级、错误码、调试宏和多任务输出策略，使后续模块不再直接散落调用 HAL_UART_Transmit()。