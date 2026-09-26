# STM32F407 Industrial Platform

本项目以 STM32F407VGT6 为核心，逐步实现一个基于 FreeRTOS 的工业实时数据采集与设备管理平台，同时将项目作为嵌入式软件工程实践载体，训练驱动开发、RTOS、通信、测试、调试、Git 和工程文档能力。本仓库采用 STM32 HAL、CMake、Ninja 和 Arm GNU Toolchain (arm-none-eabi)，逐步建立可复现、可验证、便于维护的固件工程。

当前已完成 **M2：FreeRTOS 最小系统与调度基础**，正在准备进入 **M3：线程安全、可裁剪的调试基础设施**。当前工程已经具备可重复构建、烧录和真实目标板运行能力，并完成 FreeRTOS 单任务、双任务、优先级抢占、同优先级时间片、Task Stack、FreeRTOS Heap、NRST 和完全断电重新上电验证。

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
- [ ] M3：建立线程安全、可裁剪的调试基础设施。
- [ ] M4：建立任务间通信、同步和数据缓冲基础。

## 当前构建状态

## 当前构建状态

当前 M2 Final Baseline 已通过 CMake + Ninja + GNU Arm GCC 构建，并成功生成 ELF/HEX/BIN 固件。

最近一次最终 baseline 构建资源占用：

- RAM：20992 B / 128 KB，16.02%
- FLASH：24364 B / 1 MB，2.32%

当前真实板级 baseline：

- HeartBeatTask：`osPriorityNormal`，512 B Stack，`osDelay(500U)`。
- UARTTask：`osPriorityNormal`，1024 B Stack，`osDelay(1000U)`。
- HeartBeatTask 历史最小剩余 Stack：约 384 B。
- UARTTask 历史最小剩余 Stack：约 560 B。
- FreeRTOS Current Free Heap：13592 B。
- FreeRTOS Minimum Ever Free Heap：13592 B。
- NRST ×3：通过。
- 完全断电重新上电 ×3：通过。

M2 最终功能 checkpoint 为 `9c7333b`：`feat: finalize FreeRTOS scheduling baseline`。

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
