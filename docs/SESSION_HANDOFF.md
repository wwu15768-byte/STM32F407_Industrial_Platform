# 会话交接

## 当前 Milestone

M1：STM32 裸机稳定基线。

## 当前 Branch

`feature/m1-bare-metal-baseline`，尚无远程跟踪分支。

## 已经完成并验证的内容

- M0 已完成；`main` 与 `origin/main` 指向 `4e92819`。
- M1 硬件配置输入已通过 `TEST-20260908-01` 核对并提交。
- 已确认 STM32F407VGT6/LQFP-100、HSE 8 MHz、LSE 32.768 kHz、PB2 高电平点亮、USART2 PD5/PD6、ST-LINK V2 的 SWD+NRST 和 5 V 供电约束。

## 当前正在进行的任务

准备在 STM32CubeMX 的 MCU Selector 中选择准确器件；尚未实际操作。

## 尚未完成的内容

- 未创建 `.ioc` 或生成任何固件代码。
- 未配置 SWD、RCC、时钟树、PB2 或 USART2。
- 未建立 CMake 构建目标，未编译、未下载、未运行、未完成板上功能验证。

## 当前阻塞

无。

## 已确认的重要技术决策

- 固件只使用 STM32 HAL；M1 不引入 FreeRTOS。
- CubeMX 必须选择 STM32F407VGT6，不得用 ZGT6 代替。
- 调试串口使用 USART2：PD5/TX、PD6/RX，外接 3.3 V USB-TTL。
- 外部 12 V 先降压为稳定 5 V，各设备共地，不并联 USB/ST-LINK 电源。

## 最近一次有效测试

`TEST-20260908-01`：M1 硬件配置输入核对通过；不代表固件或实物运行通过。

## 本次交接基于的 Commit

`65cd01c`：`docs: establish M1 hardware baseline`，仅存在于当前本地功能分支。

## Working Tree 状态

进入后续开发前，以 `git status --short --branch` 为准。本次交接文档提交完成后，预期工作区干净；若实际不一致，先检查差异。

## 下一步最合理的一个任务

确认工作区干净后，在 STM32CubeMX 的 MCU Selector 中准确选择 STM32F407VGT6、LQFP-100，创建最小 HAL 工程。
