# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-09-17

## Current Milestone

M1：STM32 裸机稳定基线，板级功能验收已通过，正在完成最终 Git checkpoint 与远程同步。

## Current Goal

固化已经通过真实 STM32F407VGT6 板级验证的 M1 Known Good Baseline：同步测试记录与项目状态，完成最终 Git checkpoint 并推送远程。完成后关闭 M1，再进入 M2 FreeRTOS 最小系统。

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

## In Progress

M1 收尾。板级功能验收已经完成，目前正在同步测试记录、项目状态和开发日志，并准备建立最终稳定 Git checkpoint 与远程备份。

## Blocked

无。

## Known Bugs

无已确认固件 Bug。USART2 RX 尚未进行板级验证，但属于当前验证边界，不作为已知 Bug。

## Current Branch

`feature/m1-bare-metal-baseline`，已关联远程 `origin/feature/m1-bare-metal-baseline`。本地包含 `bacda60` 文档清理 checkpoint，并正在形成 M1 最终板级验证 checkpoint；`main` 与 `origin/main` 仍停留在 `4e92819`。

## Last Verified Function

M1 STM32F407VGT6 裸机板级基线已经通过实际运行验证：
- CMake + Ninja + GNU Arm GCC 构建成功。
- ST-LINK 下载成功。
- PB2 USER_LED 能够按照设定的 500 ms 延时周期稳定翻转。
- USART2 TX 使用 PD5、115200 8N1，通过 3.3 V USB-TTL 在 PC 串口终端实际收到 `USART2 TX OK`。
- 连续 3 次 NRST 后 LED 与 USART2 TX 均正常恢复。
- 连续 3 次完全断电重新上电后 LED 与 USART2 TX 均正常恢复。

当前证据不包含 USART2 RX、精确 500 ms 时序、长期稳定性、DMA 或 FreeRTOS 验证。

## Next Step

完成 M1 测试记录和状态文档同步，建立最终 Known Good Baseline Git checkpoint 并推送到远程。确认工作区干净、远程分支同步后关闭 M1。下一阶段进入 M2，只建立最小 FreeRTOS 启动与任务调度基线。
