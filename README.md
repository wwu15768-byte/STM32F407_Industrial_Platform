# STM32F407 Industrial Platform

本项目以 STM32F407VGT6 为核心，逐步实现一个基于 FreeRTOS 的工业实时数据采集与多总线通信平台，并补充 MCU 工程中常见的数据采集、CAN、Bootloader、故障诊断和系统可靠性能力。同时将项目作为嵌入式软件工程实践载体，训练驱动开发、RTOS、通信、测试、调试、Git 和工程文档能力。本仓库采用 STM32 HAL、CMake、Ninja 和 Arm GNU Toolchain (arm-none-eabi)，逐步建立可复现、可验证、便于维护的固件工程。

当前已完成 **M0～M6** 并通过 PR #7（`08d5b64`）合入 `main`，在 `feature/m7-baseline` 上完成了 **M7-P0 运行时清理与可复用字节环模块化**（`88c3712`），准备正式进入 **M7：RS485 半双工通信**。当前固件已具备可重复构建、烧录与真实板级运行能力，并持续运行 FreeRTOS 三任务基线、线程安全同步 UART Logger、50 Hz ADC1+TIM2+DMA2 双半区采集，以及 USART2 Circular DMA + 独立字节环 + 非阻塞错误恢复链路。真实 NE 硬件注入、真实 DMA 硬件故障专项、UART TX DMA、RS485、Modbus、CAN、Bootloader 与 Watchdog 尚未实现或暂缓。

## 硬件与软件约束

- 开发板：立创·天空星 STM32F407VGT6 高配版。
- 官方资料：[产品页](https://lckfb.com/project/detail/lckfb-lspi-skystar-stm32f407vgt6-pro?param=baseInfo)、[开发板文档](https://wiki.lckfb.com/zh-hans/fdb/skystar.html)。
- 目标 MCU：STM32F407VGT6（ARM Cortex-M4F，LQFP-100）。
- 时钟：HSE 8 MHz；LSE 32.768 kHz，已安装。
- 用户 LED：PB2/BOOT1，高电平点亮，上电默认保持低电平熄灭。
- 调试串口：USART2，PD5/USART2_TX、PD6/USART2_RX，外接 3.3 V USB-TTL。
- 下载调试：ST-LINK V2 STM8&STM32，使用 SWD，并连接 NRST。
- 供电：外部 12 V 经降压模块输出稳定 5 V，再接开发板 5 V 输入；所有设备共地，不同时由 USB 或 ST-LINK 供电。
- 固件库：必须使用 STM32 HAL；除非另有明确要求，不使用 SPL 或 LL。
- 语言标准：C99/C11，兼顾嵌入式内存限制和字节对齐。
- 实现原则：保持简单、模块低耦合、检查 HAL 返回值、ISR 短小且不执行阻塞操作。
- 硬件配置：引脚、外设复用和时钟树必须在信息明确后配置，不预设开发板连接。
- RTOS：FreeRTOS，应用层接口采用 CMSIS-RTOS2；HAL Timebase 使用 TIM6，SysTick 用于 FreeRTOS Kernel Tick。

## 开发工具链

以下版本是 M0 阶段已验证的本机开发基线：

| 工具 | 用途 | 已验证版本 |
| --- | --- | --- |
| STM32CubeMX | 生成 STM32 初始化代码和工程配置 | 6.11.1 |
| STM32CubeCLT | 提供 STM32 命令行开发工具集合 | 1.22.0 |
| CMake | 生成构建系统 | 4.3.1 |
| Ninja | 执行增量构建 | 1.13.2 |
| GNU Arm GCC | 编译、汇编和链接 Cortex-M 固件 | 14.3.1 |
| STM32CubeProgrammer CLI | 烧录和读取目标器件 | 随 CubeCLT 提供 |
| ST-LINK GDB Server | 调试接口 | 随 CubeCLT 提供 |
| Git | 版本控制 | 2.54.0 |

当前已验证的构建与烧录流程：

```text
STM32CubeMX
    ↓ 生成工程源码与 .ioc 配置
CMake
    ↓ 生成 Ninja 构建规则
Ninja
    ↓ 调用 GNU Arm GCC 驱动
编译 / 汇编 / 链接
    ↓
arm-none-eabi-objcopy / arm-none-eabi-size
    ↓ 生成并检查 .elf / .bin / .hex
STM32CubeProgrammer / ST-LINK
    ↓
STM32F407VGT6
```

## 当前目录

```text
STM32F407_Industrial_Platform/
├── Inc/                         # 应用和生成头文件
├── Src/                         # 应用和生成源码
├── Drivers/                     # STM32 HAL / CMSIS
├── Middlewares/                 # FreeRTOS 与 CMSIS-RTOS2
├── cmake/                       # CubeMX/CMake 构建配置
├── docs/                        # 项目管理、测试和开发记录
├── .gitattributes               # 文本行尾规范
├── .gitignore                   # Git 忽略规则
├── CMakeLists.txt               # CMake 工程入口
├── CMakePresets.json            # 构建 preset
├── STM32F407_Industrial_Platform.ioc
├── STM32F407xx_FLASH.ld
├── startup_stm32f407xx.s
├── AGENTS.md
└── README.md
```

CubeMX 工程输入、HAL/CMSIS、FreeRTOS、启动文件、链接脚本、`.ioc` 和 CMake 配置均已纳入版本控制；`build/`、`.elf`、`.hex`、`.bin` 等可重新生成的构建产物继续由 `.gitignore` 排除。

## 里程碑进度

- [x] M0：建立开发环境、项目管理体系和稳定 Git 基线。
- [x] M1：建立可下载、可运行、可观测、可重复启动的 STM32F407VGT6 裸机 Known Good Baseline。
- [x] M2：完成 FreeRTOS 最小系统、CMSIS-RTOS2、单任务、双任务、优先级抢占、同优先级时间片、Task Stack 和 FreeRTOS Heap 验证。
- [x] M3：建立并验证线程安全、可裁剪的同步 UART 调试基础设施。
- [x] M4：完成 Queue、Thread Flags、Binary Semaphore、Mutex、TIM7 ISR 通知与 Task↔Task SPSC Ring Buffer 验证（PR #5 `e793039`）。
- [x] M5：建立并验证 ADC1 + TIM2 TRGO (50 Hz) + DMA2 Circular 实时数据采集子系统（PR #6 `30b0072`）。
- [x] M6：建立并验证 USART2 RX Circular DMA + 字节 Ring Buffer + 非阻塞 UART 错误恢复基线（PR #7 `08d5b64`；真实 NE 硬件注入、真实 DMA 故障专项与 UART TX DMA 暂缓/未覆盖）。
- [ ] **M7（当前）**：实现 RS485 半双工通信（已完成 M7-P0 运行时清理与 `byte_ring_buffer` 模块化）。
- [ ] M8：实现基础 Modbus RTU 通信。
- [ ] M9：建立统一数据模型。
- [ ] M10：建立 bxCAN 基础通信与实时收发模型。
- [ ] M11：建立 UART 或 RS485 Bootloader / IAP 基线。
- [ ] M12：建立 Watchdog、Reset Cause 与 Fault Diagnostics 可靠性基础。
- [ ] M13：打通 PC、Node-RED 和 InfluxDB 基础链路。
- [ ] M14：完成系统集成、异常注入和稳定性验证。
- [ ] M15：完成工程收尾、演示和项目表达材料。

## 当前构建状态

当前 M7-P0 清理基线（`88c3712`）已通过 CMake + Ninja + GNU Arm GCC Clean Build（45/45，0 warning / 0 error），并成功生成 ELF/HEX/BIN 固件。

最近一次 Debug Clean Build 资源占用：

- RAM：23000 B / 128 KB，17.55%
- CCMRAM：0 B / 64 KB，0.00%
- FLASH：42448 B / 1 MB，4.05%（`text=42344 B`、`data=96 B`、`bss=22904 B`）

当前真实板级运行基线（3 个应用任务 + 2 个独立基础模块 `debug_log`、`byte_ring_buffer`）：

- `HeartBeatTask`：`osPriorityNormal`，512 B Stack，`osDelay(500U)`，历史最小剩余 Stack：`384 B`。
- `UARTTask`：`osPriorityNormal`，1024 B Stack，`osDelay(10U)`，首次格式化日志后历史最小剩余 Stack 稳定为 `328 B`；负责消费 USART2 字节环、执行非阻塞 RX 异常恢复状态机，并每 5 秒输出 4 行系统/ADC/RX 摘要（收到数据时另打印 `[RX] recv=... B`，发生错误或恢复状态切换时事件触发 `[M6ERR]`）。
- `AcquisitionTask`：`osPriorityAboveNormal`，1024 B Stack，`osThreadFlagsWait`，历史最小剩余 Stack：`816 B`；处理 ADC1_IN1 (PA1) + TIM2 TRGO (50 Hz) + DMA2 Stream0 Circular (20×`uint16_t`) 半/全缓冲通知并计算 10 点平均电压。
- FreeRTOS Current Free Heap：`12368 B`；Minimum Ever Free Heap：`12368 B`。
- M7-P0 回归验证：PA1 电位器电压变化（`0～2060 mV`，`Late=0 Err=0`）、USART2 短包接收（`16 B` / `8 B`，`Drop=0 Err=0`）、NRST ×3 恢复均通过（详见 [`TEST_RECORD.md`](docs/TEST_RECORD.md) 的 `TEST-20261126-01`）。

关键里程碑 checkpoint：

- M6 合并基线（`main`）：`08d5b64`（`Merge pull request #7 from wwu15768-byte/feature/m6-baseline`，含代码 `8bc0a8e` 与文档 `e1461b6`）。
- M7-P0 清理基线（`feature/m7-baseline`）：`88c3712`（`refactor: modularize byte ring buffer and simplify runtime logging`，清理前保护标签 `m7-pre-cleanup-20261121`）。

当前未实现或暂缓的能力包括：真实 NE 硬件注入、真实 DMA 硬件故障/异步 Abort 专项、UART TX DMA、RS485、Modbus、CAN、Bootloader、Watchdog 与 HardFault Diagnostic；这些能力按 [`ROADMAP.md`](docs/ROADMAP.md) 的依赖顺序逐阶段实现。

## 文档

| 文件 | 唯一回答的问题 | 更新时机 |
| --- | --- | --- |
| [`ROADMAP.md`](docs/ROADMAP.md) | 整个项目以后往哪走？ | 里程碑变化 |
| [`PROJECT_STATUS.md`](docs/PROJECT_STATUS.md) | 项目现在在哪？ | 状态明显变化 |
| [`WEEKLY_PLAN.md`](docs/WEEKLY_PLAN.md) | 这周准备干什么？ | 每周 |
| [`DAILY_LOG.md`](docs/DAILY_LOG.md) | 今天实际上干了什么？ | 每个开发日 |
| [`DECISIONS.md`](docs/DECISIONS.md) | 为什么这么设计？ | 重要决策发生 |
| [`ERROR_LOG.md`](docs/ERROR_LOG.md) | 什么坏了，为什么，怎么修？ | 出现有学习价值的问题 |
| [`TEST_RECORD.md`](docs/TEST_RECORD.md) | 怎么证明它工作？ | 正式验证时 |
| [`KNOWLEDGE_GAPS.md`](docs/KNOWLEDGE_GAPS.md) | 我还有什么不会？ | 发现或解决知识缺口 |
| [`SESSION_HANDOFF.md`](docs/SESSION_HANDOFF.md) | 下一次会话从哪里继续？ | 会话切换前 |

## 实现原则

CubeMX 管理的文件应遵循代码生成边界；需要保留的自定义代码优先放入 USER CODE 区域或独立模块，避免在重新生成代码时丢失修改。
