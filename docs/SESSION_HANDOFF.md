# 会话交接

## 当前 Milestone

M1：STM32 裸机稳定基线。

## 当前 Branch

feature/m1-bare-metal-baseline，已关联远程 origin/feature/m1-bare-metal-baseline。

## 已经完成并验证的内容

- M0 已完成；main 与 origin/main 指向 4e92819。
- M1 硬件配置输入已通过 TEST-20260908-01 核对。
- 已确认 STM32F407VGT6/LQFP-100、HSE 8 MHz、LSE 32.768 kHz、PB2 高电平点亮、USART2 PD5/PD6、ST-LINK V2 的 SWD+NRST 和 5 V 供电约束。
- 已在 STM32CubeMX 中选择 STM32F407VGT6，并生成最小 HAL + CMake 裸机工程。
- 已核对生成配置：SYSCLK/HCLK 168 MHz、APB1 42 MHz、APB2 84 MHz，PB2 为 USER_LED，USART2 使用 PD5/PD6、115200、8N1。
- 当前工程已经通过 CMake + Ninja + GNU Arm GCC 构建，并成功生成 .elf、.hex、.bin。
- CubeMX 裸机工程已经形成独立 Git checkpoint，并推送到远程功能分支。

## 当前正在进行的任务

准备对当前可构建裸机基线进行第一次板上运行验证。当前尚未进入 FreeRTOS。

## 尚未完成的内容

- 尚未使用 ST-LINK 对当前固件完成正式下载验证。
- 尚未通过实物验证 PB2 LED 心跳。
- 尚未通过 USB-TTL 验证 USART2 实际输出。
- 尚未验证多次复位后的稳定启动行为。
- M1 尚未达到 Known Good Baseline 验收标准。
- 尚未进入 M2 FreeRTOS。

## 当前阻塞

无。

## 已确认的重要技术决策

- 固件只使用 STM32 HAL；M1 不引入 FreeRTOS。
- CubeMX 必须选择 STM32F407VGT6，不得用 ZGT6 代替。
- 调试串口使用 USART2：PD5/TX、PD6/RX，外接 3.3 V USB-TTL。
- 外部 12 V 先降压为稳定 5 V，各设备共地，不并联 USB/ST-LINK 电源。
- M1 主构建基线采用 CubeMX + CMake + Ninja + GNU Arm GCC；build/ 和 .elf/.hex/.bin 等构建产物不进入 Git。

## 最近一次有效测试

TEST-20260915-01：CubeMX 生成配置核对通过。
TEST-20260915-02：CMake + Ninja + GNU Arm GCC 构建基线通过，可生成 .elf、.hex、.bin；该结果不代表板上运行通过。

## 本次交接基于的 Commit

`4fba6ba`：feat: add STM32F407VGT6 CubeMX bare-metal project。

## Working Tree 状态

进入后续开发前，以 git status --short --branch 为准。本次文档同步提交并推送完成后，预期工作区干净，当前功能分支与远程 tracking branch 同步；若实际不一致，先检查差异。

## 下一步最合理的一个任务

不要引入 FreeRTOS。先使用 ST-LINK 验证目标连接与固件下载，然后实现最小 PB2 LED 心跳，确认裸机程序能够在 STM32F407VGT6 上稳定运行。
