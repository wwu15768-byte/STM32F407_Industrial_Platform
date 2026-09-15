# 项目状态

> 唯一回答：项目现在在哪？
> 更新时机：项目状态发生明显变化时。

更新时间：2026-09-15

## Current Milestone

M1：STM32 裸机稳定基线，已开始。

## Current Goal

完成 M1 板上运行验证：通过 ST-LINK 下载当前裸机基线，验证 PB2 LED、USART2 调试输出和复位运行，建立 Known Good Baseline。

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
- 使用 CMake + Ninja + GNU Arm GCC 成功完成首次固件编译、汇编、链接和产物生成。
- 已生成 .elf、.hex、.bin 构建产物；build/ 已由 .gitignore 正确排除，不纳入版本控制。
- CubeMX 裸机工程已形成独立 Git checkpoint：feat: add STM32F407VGT6 CubeMX bare-metal project。
- feature/m1-bare-metal-baseline 已推送到远程并建立对应 upstream。

## In Progress

M1 板上运行验证。当前工程已经能够通过 GCC + CMake + Ninja 构建，但尚未通过实物运行证明 LED、USART2 和下载复位链路正常

## Blocked

无。

## Known Bugs

无已确认固件 Bug。已纠正并归档早期目标 MCU 型号不一致问题。

## Current Branch

feature/m1-bare-metal-baseline，已关联远程 origin/feature/m1-bare-metal-baseline；main 与 origin/main 仍停留在 4e92819。

## Last Verified Function

尚无板上固件功能通过验证。最近一次有效验证为 M1 构建基线：CubeMX 生成工程能够通过 CMake + Ninja + GNU Arm GCC 完成编译、汇编、链接并生成 .elf、.hex、.bin。该结果只证明工程可构建，不证明固件已经在 STM32F407VGT6 实物上正确运行。

## Next Step

使用 ST-LINK 对当前裸机基线进行板上验证。顺序为：目标识别与下载 → 最小 LED 心跳 → USART2 调试输出 → 多次复位运行检查。全部通过后再判定 M1 完成。
