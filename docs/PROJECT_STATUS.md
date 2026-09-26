# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-09-24

## Current Milestone

M2：FreeRTOS 最小系统与调度基础，已完成。当前已形成经过真实 STM32F407VGT6 板级验证的 M2 Final Baseline，下一阶段准备进入 M3：线程安全、可裁剪的调试基础设施。

## Current Goal

完成 M2 最终文档和 Git 收尾，将已经通过单任务、双任务、优先级抢占、同优先级时间片、Task Stack、FreeRTOS Heap、NRST 和完全断电重新上电验证的 FreeRTOS baseline 固化并同步到远程。完成后进入 M3，优先建立线程安全、可裁剪的 UART 日志与调试基础设施。

## Completed

- 确认仓库根目录为 `STM32F407_Industrial_Platform`。
- 验证 STM32CubeMX、STM32CubeCLT、CMake、Ninja、GNU Arm Toolchain、CubeProgrammer CLI、ST-LINK GDB Server 和 Git。
- 初始化 Git 仓库，当前分支为 `main`。
- 配置 Git 全局提交者名称、邮箱和默认初始分支。
- 建立并验证 `.gitignore`。
- 完成 README 内容评审。
- 建立并验收 docs 项目管理体系。
- 本次提交内容已通过暂存区文件清单与空白错误检查。
- 创建第一次稳定提交 `8bc8d98`。
- 建立 GitHub 私有远程仓库并配置 `origin`。
- 验证本地 `main`、本地 `origin/main` 与 GitHub 远程 `main` 指向同一提交。
- 确认保留首次提交的旧邮箱，后续提交使用新的全局 Git 邮箱。
- 核对开发板原理图、引脚分配图、厂商资料和 MCU 数据手册。
- 确认目标为立创·天空星 STM32F407VGT6 高配版，MCU 为 LQFP-100 封装。
- 确认 HSE 8 MHz、LSE 32.768 kHz、PB2 高电平点亮 LED。
- 确认 USART2 使用 PD5/PD6，外接 USB-TTL 使用 3.3 V 电平。
- 确认 ST-LINK V2 使用 SWD 并连接 NRST。
- 确认外部 12 V 经降压后以稳定 5 V 为开发板供电，各设备共地且不并联其他供电源。
- 创建 `feature/m1-bare-metal-baseline` 分支，并提交 M1 硬件基线文档 `65cd01c`。
- 在 STM32CubeMX 中选择 STM32F407VGT6、LQFP-100，生成最小 HAL + CMake 裸机工程。
- 完成 M1 配置层核对：HSE 为 8 MHz，PLL 系统时钟为 168 MHz，APB1 为 42 MHz，APB2 为 84 MHz。
- 已生成 PB2 USER_LED GPIO 输出配置，以及 USART2 PD5/TX、PD6/RX、115200、8N1 配置。
- 使用 `CMake + Ninja + GNU Arm GCC` 成功完成首次固件编译、汇编、链接和产物生成。
- 已生成 .elf、.hex、.bin 构建产物；build/ 已由 .gitignore 正确排除，不纳入版本控制。
- CubeMX 裸机工程已形成独立 Git checkpoint：`feat: add STM32F407VGT6 CubeMX bare-metal project`。
- `feature/m1-bare-metal-baseline` 已推送到远程并建立对应 upstream。
- 使用 ST-LINK 成功将当前裸机固件下载到 STM32F407VGT6，并完成真实目标板运行验证。
- PB2 USER_LED 已完成最小 heartbeat 验证，按照程序设定的 500 ms HAL 延时周期稳定翻转。
- PB2 heartbeat 在连续 3 次 NRST 后均能够正常恢复运行。
- USART2 已通过 PD5/TX、115200、8N1、无流控配置完成真实 TX 验证。
- PC 通过 3.3 V USB-TTL 实际接收到 `USART2 TX OK` 启动信息。
- USART2 TX 在连续 3 次 NRST 后均能够重新输出，且 PB2 heartbeat 同时保持正常。
- 完成连续 3 次完全断电重新上电验证，每次 PB2 heartbeat 和 USART2 TX 均能够恢复正常。
- 当前 M1 已达到可构建、可下载、可运行、可观测和可重复启动的板级功能状态。
- M1 已完成最终 Git 收尾并形成 `0b6c86f`：`docs: close M1 and prepare M2 handoff`。
- 已创建 `feature/m2-freertos-baseline` 分支并从 M1 Known Good Baseline 开始 M2。
- 将 HAL Timebase Source 从 SysTick 迁移到 TIM6，TIM6 提供 HAL 1 ms 时基，SysTick 用于 FreeRTOS Tick。
- 在 CubeMX 中启用 FreeRTOS，接口采用 CMSIS-RTOS2。
- 已加入 FreeRTOS Kernel、CMSIS-RTOS2 wrapper、ARM Cortex-M4F GCC port、`heap_4` 和 `FreeRTOSConfig.h`。
- FreeRTOS Tick 当前为 1000 Hz。
- 建立 `HeartbeatTask`，使用 PB2 USER_LED 和 `osDelay(500U)` 完成单用户任务调度验证。
- FreeRTOS 单任务版本已成功完成 CMake + Ninja + GNU Arm GCC 构建、链接、烧录和真实目标板运行。
- 单任务版本 NRST ×3 和完全断电重新上电 ×3 均通过。
- 已形成 `4d62a23`：`feat: establish FreeRTOS single-task baseline`。
- `feature/m2-freertos-baseline` 已推送到远程并建立 upstream。
- 已新增 `UARTTask`，周期通过 USART2 输出测试字符串并使用 `osDelay(1000U)` 阻塞。
- `HeartbeatTask` 与 `UARTTask` 当前均为 `osPriorityNormal`。
- 两个同优先级用户任务已经在真实硬件上完成稳定共存运行验证。
- 双任务版本 NRST ×3 和完全断电重新上电 ×3 均通过。
- 已形成并推送 `a8e588c`：`feat: establish FreeRTOS dual-task baseline`。
- 当前 M2 已达到可构建、可烧录、可启动、可观测并可重复运行的 FreeRTOS 双用户任务基线状态。
- 已通过最小实验验证不同优先级任务抢占行为，高优先级 UARTTask 能够在从 Blocked 恢复为 Ready 后抢占持续运行的低优先级任务。
- 已形成 46aef6a：feat: verify FreeRTOS priority preemption。
- 已使用 osThreadGetStackSpace() 完成 HeartbeatTask 和 UARTTask 的运行时 Stack High Water Mark 验证。
- UARTTask 初始 512 B Stack 历史最小剩余仅 64 B，因此根据实际测量将 Stack 调整至 1024 B。
- UARTTask 调整至 1024 B 后，在当前完整运行路径下历史最小剩余 Stack 稳定约为 560 B。
- 已使用 xPortGetFreeHeapSize() 和 xPortGetMinimumEverFreeHeapSize() 完成 FreeRTOS Heap 运行时观测。
- 当前 configTOTAL_HEAP_SIZE 为 15360 B，Current Free Heap 和 Minimum Ever Free Heap 均稳定为 13592 B。
- 已形成 51f143c：feat: add FreeRTOS runtime resource monitoring。
- 已核对 configUSE_PREEMPTION = 1。
- 已确认 configUSE_TIME_SLICING 当前最终有效值为 1。
- 已建立两个相同 osPriorityNormal 优先级、持续 Ready 的 Busy Task，完成同优先级时间片调度行为验证。
- BusyTaskA 和 BusyTaskB 的计数器均持续增长，证明两个同优先级持续 Ready Task 均能够持续获得 CPU 执行机会。
- 同优先级时间片实验连续 3 次 NRST 均能够重复正常运行。
- 时间片实验完成后已删除 BusyTask 测试结构并恢复正常周期任务 baseline。
- 当前最终任务结构为 HeartBeatTask + UARTTask，两个任务均使用 osPriorityNormal 并通过 osDelay() 主动阻塞。
- HeartBeatTask 使用 512 B Stack，最终 baseline 历史最小剩余约 384 B。
- UARTTask 使用 1024 B Stack，最终 baseline 历史最小剩余约 560 B。
- 最终 baseline 中 FreeRTOS Current Free Heap 和 Minimum Ever Free Heap 均稳定为 13592 B。
- M2 Final Baseline 已完成 NRST ×3 和完全断电重新上电 ×3 验证。
- 未观察到 HardFault、Hang、任务异常停止或 Heap 持续下降。
- 已清理 CubeMX/编辑器产生的大量 CRLF/LF EOL-only 工作区噪音，最终功能修改边界仅保留 .ioc 与 Src/freertos.c。
- 已形成 9c7333b：feat: finalize FreeRTOS scheduling baseline。
- 当前 M2 已达到可构建、可烧录、可运行、可观测、可重复启动并具有基础运行时资源监测能力的稳定 FreeRTOS baseline。

## In Progress

M2 最终文档和 Git 收尾。M2 功能代码与板级验证已经完成，目前正在同步测试记录、项目状态、开发日志、知识缺口、路线图、README 和会话交接；文档 checkpoint 完成后统一推送远程并关闭 M2。

## Blocked

无。

## Known Bugs

无已确认固件 Bug。

USART2 RX、Queue、Task Notification、Mutex、ADC、DMA、RS485 和 Modbus 尚未进行当前阶段验证，属于后续里程碑范围，不作为已知 Bug。

## Current Branch

`feature/m2-freertos-baseline`，已关联远程 `origin/feature/m2-freertos-baseline`。

当前 M2 最终功能 checkpoint 为 `9c7333b`：`feat: finalize FreeRTOS scheduling baseline`。

该提交创建后本地分支领先远程 1 个 commit，当前正在进行 M2 最终文档同步；文档 checkpoint 完成后再统一 push。

M1 最终分支 `feature/m1-bare-metal-baseline` 保持在 `0b6c86f`；main 与 origin/main 仍保持原稳定基线。

## Last Verified Function

M2 FreeRTOS Final Baseline 已经通过 STM32F407VGT6 真实硬件验证：
- HAL Timebase 使用 TIM6。
- FreeRTOS 使用 SysTick 作为 1000 Hz Kernel Tick。
- FreeRTOS Kernel、CMSIS-RTOS2、ARM_CM4F port 和 heap_4 已正常构建和运行。
- configUSE_PREEMPTION = 1。
- configUSE_TIME_SLICING = 1。
- 已验证一个用户任务、两个周期任务、不同优先级抢占和同优先级时间片调度行为。
- HeartBeatTask 使用 osPriorityNormal、512 B Stack 和 osDelay(500U)，持续驱动 PB2 USER_LED heartbeat。
- UARTTask 使用 osPriorityNormal、1024 B Stack 和 osDelay(1000U)，持续通过 USART2 输出状态信息。
- HeartBeatTask 历史最小剩余 Stack 约为 384 B。
- UARTTask 历史最小剩余 Stack 约为 560 B。
- FreeRTOS Current Free Heap 为 13592 B。
- FreeRTOS Minimum Ever Free Heap 为 13592 B。
- 连续 3 次 NRST 后系统均恢复正常。
- 连续 3 次完全断电重新上电后系统均恢复正常。
- 未观察到 HardFault、Hang 或运行时 Heap 持续下降。
- 当前稳定功能 checkpoint 为 9c7333b。

当前证据不包含 USART2 RX、任务间正式通信与同步、线程安全多任务日志、ADC/DMA、RS485、Modbus 或长时间压力测试。

## Next Step

完成 M2 文档关闭 checkpoint 并统一 push `feature/m2-freertos-baseline`。确认本地分支、远程 tracking branch 和工作区状态一致后关闭 M2。

下一阶段进入 M3：线程安全、可裁剪的调试基础设施。优先整理 UART 日志接口、日志等级、错误码、调试宏和多任务并发输出策略，不提前进入 ADC、DMA、RS485 或 Modbus。
