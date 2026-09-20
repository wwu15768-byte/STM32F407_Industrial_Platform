# 每日开发日志

> 唯一回答：今天实际上干了什么？
> 更新时机：每个有效开发日结束时，简洁记录事实。

# 2026-09-08

## 今日计划

- 完成 M1 硬件信息确认。
- 纠正项目中的目标 MCU 型号。
- 为 CubeMX 最小工程准备可靠配置输入。

## 实际完成

- 核对开发板原理图、引脚分配图、厂商资料和 STM32F407 数据手册。
- 确认开发板为立创·天空星 STM32F407VGT6 高配版，MCU 为 STM32F407VGT6、LQFP-100。
- 确认 HSE 8 MHz、LSE 32.768 kHz、PB2/BOOT1 LED 高电平点亮。
- 确认 PD5/PD6 分别为 USART2_TX/USART2_RX，外接 USB-TTL 使用 3.3 V 电平。
- 确认 ST-LINK V2 使用 SWD 并连接 NRST。
- 确认外部 12 V 经降压模块转换为稳定 5 V 后供电，设备共地且不并联 USB/ST-LINK 电源。
- 发现并纠正原项目文档中的 STM32F407ZGT6 型号假设。
- 创建 `feature/m1-bare-metal-baseline` 分支并提交硬件基线文档，提交为 `65cd01c`。
- 检查仓库与项目记录，建立本次会话交接文档。

## 验收情况

- M1 硬件配置输入：已完成文档核对。
- M1 硬件基线提交：已检查分支、提交内容、提交身份和干净工作区。
- 固件工程：尚未生成、编译或下载。

## 未完成

- CubeMX 最小 HAL 工程生成与配置审查。
- CMake + Ninja 构建、下载和板上运行验证。

## 今日 Bug

无已确认固件 Bug；发现并纠正一项配置错误：原项目目标型号与实际硬件不一致，详见 `ERR-20260908-01`。

## 今日学习

- STM32F407VGT6 与 STM32F407ZGT6 的封装和引脚数量不同，不能混用 CubeMX 器件配置。
- PD5/PD6 属于 USART2；板载调试座的 USART1 与其不是同一接口。
- 原理图可以同时验证信号连接、LED 有效电平和板端允许的供电入口。

## 尚未理解

- 尚未通过实际 CubeMX 配置验证系统时钟树和各总线频率。

## Git

- Branch：`feature/m1-bare-metal-baseline`
- Commit：`65cd01c`，`docs: establish M1 hardware baseline`

## 明天下一步

确认工作区干净后进入 CubeMX 器件选择。

# 2026-09-06

## 今日计划

- 确认开发环境。
- 初始化 Git 并建立忽略规则。
- 建立 README 与 docs 项目管理体系。
- 为第一次稳定提交做好准备。

## 实际完成

- 确认 STM32CubeMX + CMake + Ninja + GNU Arm Toolchain + CubeProgrammer/ST-LINK 工具链可用。
- 初始化本地 Git 仓库，设置默认分支 `main`，确认提交者配置。
- 创建并验证 `.gitignore`。
- 完成 README 编写与内容复核。
- 建立并验收 8 个 docs 管理文件。
- 创建第一次稳定提交 `8bc8d98`。
- 建立 GitHub 私有远程仓库，配置并推送 `origin/main`。
- 通过 `git ls-remote` 验证 GitHub 远程 `main` 的提交哈希。
- 确认旧提交保留旧邮箱，后续提交使用 Gmail 身份。

## 验收情况

- 开发环境：已验证。
- Git 初始化：已验证。
- `.gitignore`：已通过示例路径验证。
- README：已复核通过。
- docs：已完成内容验收。
- 第一次稳定提交：已创建并完成提交后检查。
- 远程仓库：已完成首次推送和远程哈希验证。
- M0：验收通过。

## 未完成

- M1 尚未开始。

## 今日 Bug

无已确认项目 Bug。

## 今日学习

- Git 仓库、工作区和忽略规则的基本作用。
- `.gitignore` 对未跟踪文件生效，不会自动取消已跟踪文件。
- Git 全局配置与仓库配置的作用范围不同。
- `git ls-remote` 会直接读取远程引用，`origin/main` 是本地保存的远程跟踪引用。
- 修改 Git 身份不会追溯修改已有提交。

## 尚未理解

- 暂存区与提交快照的完整检查流程仍待实际操作。

## Git

- Branch：`main`
- Commit：`8bc8d98`，以及本次 M0 收尾文档提交
- Remote：`origin/main`

## 明天下一步

进入 M1，先确认硬件型号、时钟源、LED、UART 和 ST-LINK 连接信息。

# 2026-09-15

## 今日计划

- 修复 M1 开发进度与 Git 历史不同步的问题。
- 将已经完成的 `CMake + Ninja + GCC` 构建结果同步到项目状态和测试记录。
- 将当前功能分支推送到远程，恢复本地与远程备份节奏。

## 实际完成

- 检查 git status、当前 branch、最近提交、远程仓库和 upstream 状态。
- 确认当前位于 `feature/m1-bare-metal-baseline`。
- 使用 `git status --ignored --short` 和 `git check-ignore` 思路确认 `build/` 已正确被忽略。
- 检查 `.mxproject`，确认其中主要记录 CubeMX 相对工程路径，没有发现需要排除的机器私有绝对路径。
- 将 .ioc、CMake 配置、HAL/CMSIS、启动文件、链接脚本和生成源码作为同一个 CubeMX 工程 checkpoint 纳入 Git。
- 在提交前通过 `git status` 和 `git diff --cached --stat` 检查暂存快照，未将 build/、.elf、.hex 或 .bin 纳入提交。
- 创建提交 `feat: add STM32F407VGT6 CubeMX bare-metal project`。
- 将 `feature/m1-bare-metal-baseline` 推送到远程并建立 upstream。
- 复核并记录现有 `CMake + Ninja + GNU Arm GCC` 构建成功结果。
- 同步更新 M1 项目状态、会话交接、测试记录和知识缺口。

## 验收情况

- CubeMX 工程 Git checkpoint：完成。
- 构建产物忽略规则：确认有效。
- M1 构建基线：已通过 `CMake + Ninja + GCC` 验证。
- 远程功能分支：已建立并同步。
- 板上运行：尚未验证，因此 M1 尚未完成。

## 未完成

- ST-LINK 实际下载验证。
- PB2 LED 心跳验证。
- USART2 实际串口输出验证。
- 多次复位后的稳定启动验证。
- M1 最终验收与后续合并。

## 今日 Bug

- 无已确认固件 Bug。发现开发进度已经明显领先于 Git checkpoint 和项目状态文档，属于工程流程同步问题；本次通过补充有意义的 commit、远程 push 和状态文档同步进行修正，不单独建立 ERROR_LOG。

## 今日学习

- Git commit 用于保存有意义的本地工程状态，push 用于将已有 commit 同步到远程，两者作用不同。
- 第三方 HAL/CMSIS 源码可以作为工程输入进入仓库，但 build/、.elf、.hex、.bin 等可重新生成的构建产物不应进入版本控制。
- 一个 CubeMX 工程首次进入 Git 时文件数量较多属于正常现象，重点应检查提交边界而不是文件数量。
- “代码已生成”、“工程可编译”、“固件可运行”、“功能已验证”是四个不同状态，不能混为一谈。
- Git checkpoint 应在形成有意义的工程状态时主动建立，而不能等到大量修改累积后再一次性补交。

## 尚未理解

- 尚未通过实际 ST-LINK 下载和目标板运行观察构建出的固件。
- 尚未验证当前时钟配置、PB2 GPIO 和 USART2 配置在真实硬件上的行为。

## Git

- Branch：feature/m1-bare-metal-baseline
- Commit：`4fba6ba`，`feat: add STM32F407VGT6 CubeMX bare-metal project`
- Remote：origin/feature/m1-bare-metal-baseline

## 明天下一步

- 保持 M1，不引入 FreeRTOS。先通过 ST-LINK 完成固件下载和最小 LED 心跳验证，然后再验证 USART2 调试输出。

# 2026-09-17

## 今日计划

- 清理 M1 开发前遗留的文档修改并恢复干净工作区。
- 完成 STM32F407VGT6 裸机基线的第一次真实板级运行验证。
- 验证 ST-LINK 下载、PB2 LED heartbeat、USART2 TX、NRST 和断电重启行为。
- 在验证通过后建立 M1 Known Good Baseline。

## 实际完成

- 检查当前分支、HEAD、远程跟踪分支和暂存区状态。
- 清理 `DAILY_LOG.md`、`PROJECT_STATUS.md` 和 `TEST_RECORD.md` 中的文档修改，并创建提交 `bacda60`：`docs: clean up M1 records`。
- 在 PB2 上加入最小 LED heartbeat 测试逻辑，使用设定的 500 ms HAL 延时周期翻转 USER_LED。
- 使用 CMake + Ninja + GNU Arm GCC 成功重新构建固件。
- 使用 ST-LINK 成功将固件下载到 STM32F407VGT6。
- 实物确认 PB2 USER_LED 能够稳定周期翻转。
- 连续执行 3 次 NRST，LED heartbeat 均能够正常恢复运行。
- 核对 USART2 配置为 PD5/TX、PD6/RX、115200、8N1、无硬件流控。
- 使用 `HAL_UART_Transmit()` 完成最小 USART2 TX 启动信息发送测试。
- PC 通过 3.3 V USB-TTL 实际接收到 `USART2 TX OK`。
- 连续执行 3 次 NRST，每次均能重新收到启动信息，同时 LED heartbeat 保持正常。
- 连续完成 3 次完全断电重新上电测试，每次 LED heartbeat 和 USART2 TX 均恢复正常。
- 完成 M1 主要板级功能验收，开始整理测试记录和最终 Git checkpoint。

## 验收情况

- CMake + Ninja + GCC 构建：通过。
- ST-LINK 固件下载：通过。
- STM32F407VGT6 启动运行：通过。
- PB2 USER_LED heartbeat：通过。
- USART2 TX：通过。
- NRST ×3：通过。
- 完全断电重新上电 ×3：通过。
- USART2 RX：本次未测试，不属于当前 M1 最小验收范围。
- M1 板级功能验收：通过，等待最终文档同步、Git checkpoint 和远程 push 后关闭 M1。

## 未完成

- 完成 M1 最终状态文档同步。
- 建立并检查 M1 稳定 Git checkpoint。
- 将最终 M1 checkpoint 推送到远程。
- USART2 RX 留待后续正式通信阶段验证。

## 今日 Bug

- 无需要归档的固件 Bug。
- UART 测试代码初次编译时出现一次缺少分号的 C 语法错误；根据 GCC 报错定位到上一行并立即修正，未形成持续性故障，因此不单独建立 ERROR_LOG。
- 执行 CMake 命令时曾将 `--verbose` 误输入为 `--verpose`，属于一次性命令输入错误。

## 今日学习

- “配置存在”、“代码可编译”、“固件可下载”和“功能经过真实硬件验证”属于不同工程状态。
- GCC 报告 `expected ',' or ';' before ...` 时，实际语法错误可能位于报错位置之前，应优先检查前一条语句是否正确结束。
- USART2 初始化成功不等于 UART 通信已经通过验证，必须使用真实接收端观察数据。
- 当前测试只证明 USART2 TX 链路，不能据此认为 RX 也已通过。
- NRST 验证和完全断电重新上电验证覆盖的启动场景不同。
- 一个 Known Good Baseline 必须同时具有可构建、可下载、可运行、可观测和可重复验证的证据。

## 尚未理解

- USART2 RX 尚未进行实际板级验证。
- 尚未验证当前系统长期运行稳定性和精确时序。
- 尚未开始 FreeRTOS 调度行为的实际验证。

## Git

- Branch：`feature/m1-bare-metal-baseline`
- 开发起点 Commit：`bacda60`，`docs: clean up M1 records`
- M1 最终功能 checkpoint：待创建
- Remote：最终 checkpoint 创建后待 push

## 明天下一步

完成 M1 Git checkpoint 和远程同步后，冻结裸机 Known Good Baseline。下一项目阶段进入 M2，但首先只建立最小 FreeRTOS 启动与任务调度基线，不直接进入 ADC、DMA 或正式通信功能。

# 2026-09-20

## 今日计划

- 从已经完成的 M1 STM32F407VGT6 裸机 Known Good Baseline 开始推进 M2。
- 最小化引入 FreeRTOS，建立可构建、可烧录、可运行的单任务调度基线。
- 在单任务基线稳定后增加第二个用户任务，验证两个同优先级任务能够稳定共存运行。
- 为 M2 建立有意义的 Git checkpoint 和远程恢复点。

## 实际完成

- 确认当前位于 `feature/m2-freertos-baseline`，开发起点为 M1 最终提交 `0b6c86f`，工作区干净。
- 确认 M2 开始前仓库中不存在 FreeRTOS 内核、CMSIS-RTOS 适配层或 TIM6 HAL 时基代码。
- 将 HAL Timebase Source 从 SysTick 迁移到 TIM6，使 TIM6 提供 HAL 1 ms 时基，SysTick 用于 FreeRTOS Tick。
- 在 CubeMX 中启用 FreeRTOS，接口采用 CMSIS-RTOS2。
- 引入 FreeRTOS Kernel、ARM Cortex-M4F GCC port、CMSIS-RTOS2 wrapper、`heap_4` 和 `FreeRTOSConfig.h`。
- 建立最小 `HeartbeatTask`，使用 PB2 USER_LED 和 `osDelay(500U)` 验证任务运行和阻塞延时。
- 使用 CMake + Ninja + GNU Arm GCC 成功完成 FreeRTOS 工程配置、编译、链接和固件产物生成。
- 确认 `.elf`、`.hex`、`.bin` 均成功生成并可以正常烧录。
- 单任务版本在真实 STM32F407VGT6 上验证通过，LED heartbeat 正常。
- 单任务版本连续执行 3 次 NRST 和 3 次完全断电重新上电，均正常恢复。
- 创建并推送提交 `4d62a23`：`feat: establish FreeRTOS single-task baseline`。
- 为 `feature/m2-freertos-baseline` 建立远程 upstream。
- 新增 `UARTTask`，将 USART2 周期发送逻辑从 `main()` 迁移到 FreeRTOS 任务。
- `HeartbeatTask` 与 `UARTTask` 均使用 `osPriorityNormal`。
- `HeartbeatTask` 使用 `osDelay(500U)`，`UARTTask` 使用 `osDelay(1000U)`。
- 双任务版本成功完成增量构建、链接和烧录。
- 实物确认 LED heartbeat 与 USART2 周期输出能够同时稳定运行。
- 双任务版本连续执行 3 次 NRST 和 3 次完全断电重新上电，均正常恢复。
- 创建并推送提交 `a8e588c`：`feat: establish FreeRTOS dual-task baseline`。
- 本地 `feature/m2-freertos-baseline` 与 `origin/feature/m2-freertos-baseline` 已同步，工作区干净。

## 验收情况

- M1 Known Good Baseline 继承：通过。
- HAL Timebase 从 SysTick 迁移到 TIM6：通过。
- FreeRTOS + CMSIS-RTOS2 集成：通过。
- FreeRTOS Kernel、ARM_CM4F port 和 `heap_4` 构建：通过。
- 单用户任务 scheduler 运行：通过。
- `HeartbeatTask + osDelay(500U)`：通过。
- 双用户任务稳定共存：通过。
- `UARTTask + osDelay(1000U)`：通过。
- CMake + Ninja + GCC 构建：通过。
- ELF/HEX/BIN 生成：通过。
- 固件烧录：通过。
- NRST ×3：通过。
- 完全断电重新上电 ×3：通过。
- M2 单任务与双任务 Git checkpoint：已创建并推送远程。
- 不同优先级抢占、精确时间片行为、任务同步与通信：尚未验证。

## 未完成

- 尚未通过专门实验验证不同优先级任务之间的抢占行为。
- 尚未精确验证同优先级任务的时间片轮转行为。
- 尚未测量 FreeRTOS heap 的运行时剩余量。
- 尚未测量 HeartbeatTask 和 UARTTask 的实际栈余量。
- 尚未进入 Queue、Task Notification、Mutex 等任务间通信与同步机制。
- USART2 RX 尚未进行真实板级验证。

## 今日 Bug

- 无需要归档到 `ERROR_LOG.md` 的项目 Bug。
- CubeMX 生成的 `cmake/stm32cubemx/CMakeLists.txt` 曾存在一处 trailing whitespace，提交前通过 `git diff --check` 发现并清理。
- Git 检查过程中多次出现 CRLF 将被规范化为 LF 的提示，目前未造成源码功能或构建异常。
- 曾将 `git status` 误输入为 `giy status`，属于一次性命令输入错误。
- 新生成的 `.clangd` 仅包含 `build/Debug` 编译数据库路径，不属于当前 FreeRTOS 功能 checkpoint。

## 今日学习

- FreeRTOS 启动 scheduler 后，主要业务执行主体从裸机 `main()` 无限循环转移到各个 Task。
- `osDelay()` 会使当前任务进入 Blocked 状态，而不是通过忙等持续占用当前执行机会。
- 当所有用户任务都处于 Blocked 状态时，FreeRTOS 自动创建的 Idle Task 可以运行。
- FreeRTOS 的 SysTick 负责内核 Tick；STM32 HAL 时基可以迁移到 TIM6，从而分离 HAL Tick 与 RTOS Tick 的职责。
- SVC、PendSV 和 SysTick 是 Cortex-M FreeRTOS port 实现调度和上下文切换的重要异常机制。
- `defaultTask` 只是 CubeMX 创建的普通用户任务，不等于 FreeRTOS Idle Task。
- 同优先级任务都处于 Ready 状态时，调度器可以通过时间片、阻塞、唤醒或 yield 等调度点切换任务。
- 高优先级 Ready Task 才具有对低优先级 Running Task 的优先级抢占能力。
- `configTOTAL_HEAP_SIZE` 对应 FreeRTOS 自身的 heap，与链接脚本中的传统 C heap 不是同一个概念。
- 链接器报告的静态 RAM 占用不能直接代表运行时 FreeRTOS heap 和 Task Stack 的实际使用量。
- Git 中普通 `git diff` 不包含未跟踪文件；新增中间件时必须结合 `git status` 和暂存后的 `git diff --cached` 检查真实提交边界。
- FreeRTOS 集成应分阶段建立单任务和双任务稳定 checkpoint，避免一次引入过多变量。

## 尚未理解

- 不同优先级任务从 Blocked 转为 Ready 时，抢占和 PendSV 上下文切换的完整时序仍需通过实验深化。
- 同优先级任务时间片轮转的具体实现与切换条件仍需继续学习。
- 尚未掌握 Task Stack High Water Mark 的实际测量和栈大小调整方法。
- 尚未掌握 FreeRTOS heap 当前剩余量、最小剩余量和内存分配失败的监测方法。
- 尚未实际处理多个任务竞争同一个共享资源时的同步问题。

## Git

- Branch：`feature/m2-freertos-baseline`
- M2 起点：`0b6c86f`，`docs: close M1 and prepare M2 handoff`
- 单任务 checkpoint：`4d62a23`，`feat: establish FreeRTOS single-task baseline`
- 双任务 checkpoint：`a8e588c`，`feat: establish FreeRTOS dual-task baseline`
- Remote：`origin/feature/m2-freertos-baseline`
- 当前本地分支与远程 tracking branch 已同步。
- Working Tree：clean。

## 明天下一步

继续 M2，不进入 ADC、DMA 或正式通信模块。先通过最小实验验证不同优先级任务之间的抢占行为，并学习和测量 Task Stack 与 FreeRTOS heap 的运行时使用情况；完成这些基础验证后，再进入任务间通信与同步机制。