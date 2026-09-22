# STM32F407 Industrial Platform

本项目以 STM32F407VGT6 为核心，逐步实现一个基于 FreeRTOS 的工业实时数据采集与设备管理平台，同时将项目作为嵌入式软件工程实践载体，训练驱动开发、RTOS、通信、测试、调试、Git 和工程文档能力。本仓库采用 STM32 HAL、CMake、Ninja 和 Arm GNU Toolchain (arm-none-eabi)，逐步建立可复现、可验证、便于维护的固件工程。

当前处于 **M1：STM32 裸机稳定基线阶段**。硬件配置输入已经确认，尚未生成 STM32 工程，也不包含正式业务功能。

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
- RTOS：FreeRTOS；在基础裸机工程验证通过后引入。

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

计划中的构建与烧录流程：

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
├── .gitignore       # Git 忽略规则
├── AGENTS.md        # 项目约束和参考资料
├── README.md        # 项目入口文档
└── docs/            # 项目管理与过程记录
```

CubeMX 生成的 `Core/`、`Drivers/`、启动文件、链接脚本、`.ioc` 和 CMake 配置将在后续里程碑加入版本控制；构建目录和二进制产物不会提交。

## M0 进度

- [x] 确认仓库根目录。
- [x] 确认开发工具链。
- [x] 初始化 Git 仓库并统一默认分支为 `main`。
- [x] 建立 `.gitignore`。
- [x] 完成 README 与 docs 项目管理体系。
- [x] 由本次提交建立第一次稳定基线。
- [x] 建立私有远程仓库并验证首次推送。

## 当前构建状态

硬件基线已确认，目前仍没有可编译目标。下一步使用 STM32CubeMX 为 STM32F407VGT6 生成最小 HAL 工程，再建立并验证 CMake 配置。

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
