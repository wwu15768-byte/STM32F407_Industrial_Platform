# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-09-20

## Current Milestone

M2：FreeRTOS 最小系统与调度基础。FreeRTOS 已完成最小集成，单用户任务和双用户任务调度基线均已通过真实 STM32F407VGT6 板级验证。

## Current Goal

在已经验证的 FreeRTOS 双任务基线上继续理解和验证调度机制：完成不同优先级任务抢占实验，测量 Task Stack 与 FreeRTOS heap 的运行时余量，并为后续任务间通信与同步建立可靠基础。

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

## In Progress

M2 FreeRTOS 调度机制学习与验证。当前单任务和双任务基础运行已经完成，正在准备验证不同优先级任务之间的抢占行为，并开始建立 Task Stack 与 FreeRTOS heap 的运行时资源观测能力。

## Blocked

无。

## Known Bugs

无已确认固件 Bug。

当前尚未验证 USART2 RX、不同优先级抢占、精确时间片行为、Task Stack 余量和 FreeRTOS heap 运行时余量，这些属于当前验证边界和后续学习任务，不作为已知 Bug。

## Current Branch

`feature/m2-freertos-baseline`，已关联远程 `origin/feature/m2-freertos-baseline`。

当前 HEAD 为 `a8e588c`：`feat: establish FreeRTOS dual-task baseline`，本地分支与远程 tracking branch 已同步，工作区干净。

M1 最终分支 `feature/m1-bare-metal-baseline` 与远程均停留在 `0b6c86f`；`main` 与 `origin/main` 仍停留在 `4e92819`。

## Last Verified Function

M2 FreeRTOS 双用户任务基线已经通过 STM32F407VGT6 真实硬件验证：

- HAL Timebase 已从 SysTick 迁移到 TIM6。
- FreeRTOS 使用 SysTick 作为 1000 Hz 内核 Tick。
- FreeRTOS Kernel、CMSIS-RTOS2、ARM_CM4F port 和 `heap_4` 已成功参与实际构建。
- CMake + Ninja + GNU Arm GCC 构建成功。
- .elf、.hex、.bin 均正常生成并成功烧录。
- `HeartbeatTask` 使用 `osPriorityNormal` 和 `osDelay(500U)`，持续驱动 PB2 USER_LED heartbeat。
- `UARTTask` 使用 `osPriorityNormal` 和 `osDelay(1000U)`，持续通过 USART2 输出测试信息。
- 两个同优先级用户任务能够同时稳定运行。
- 连续 3 次 NRST 后两个任务均恢复正常。
- 连续 3 次完全断电重新上电后两个任务均恢复正常。
- 当前稳定工程 checkpoint 为 `a8e588c`。

当前证据不包含不同优先级抢占的专门验证、精确时间片测量、Task Stack/FreeRTOS heap 运行时余量、任务间同步通信以及 USART2 RX。

## Next Step

继续 M2。在当前 `a8e588c` 双任务稳定基线上设计最小优先级实验，验证高优先级 Ready Task 对低优先级 Running Task 的抢占行为；随后使用 FreeRTOS 提供的运行时接口观察任务栈和 heap 余量。完成调度和资源基础验证后，再进入 Queue、Task Notification、Mutex 等任务间通信与同步机制。
