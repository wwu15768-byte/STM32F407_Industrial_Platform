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
- `osDelay()` 会使当前任务进入 Blocked 状态，不通过忙等持续占用当前执行机会。
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

# 2026-09-24

## 今日计划

- 核对 `feature/m2-freertos-baseline` 当前真实 Git 状态。
- 完成 FreeRTOS 同优先级时间片调度实验。
- 将调度实验代码恢复为适合长期保留的 M2 Final Baseline。
- 完成最终 Task Stack、FreeRTOS Heap、NRST 和断电重启验证。
- 清理 Git 工作区中的 CRLF/LF 行尾噪音，并建立 M2 最终功能 checkpoint。
- 开始同步 M2 收尾文档。

## 实际完成

- 确认当前位于 `feature/m2-freertos-baseline`，开发起点为 `51f143c`：`feat: add FreeRTOS runtime resource monitoring`。
- 确认本地分支与 `origin/feature/m2-freertos-baseline` 同步，开发开始时工作区干净。
- 使用 `git grep` 确认：
  - `configUSE_PREEMPTION = 1`
  - `configUSE_TIME_SLICING = 1`
- 建立 BusyTaskA 和 BusyTaskB，两个任务均使用 `osPriorityNormal`，不调用 `osDelay()` 或 yield，持续递增各自计数器。
- UARTTask 使用 `osPriorityAboveNormal`，每约 1000 ms 唤醒并打印两个 Busy Task 计数器。
- 实物确认 BusyTaskA 和 BusyTaskB 计数器均持续增长，完成同优先级时间片调度行为验证。
- 连续执行 3 次 NRST，同优先级调度实验均能重复正常运行。
- 完成后删除 BusyTaskA/BusyTaskB 实验结构，恢复正常 HeartBeatTask + UARTTask baseline。
- HeartBeatTask 使用 `osPriorityNormal`、512 B Stack 和 `osDelay(500U)`。
- UARTTask 使用 `osPriorityNormal`、1024 B Stack 和 `osDelay(1000U)`。
- 保留 Task Stack 和 FreeRTOS Heap 运行时监测。
- 最终 baseline 实测：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：560 B。
  - FreeRTOS Current Free Heap：13592 B。
  - FreeRTOS Minimum Ever Free Heap：13592 B。
- 观察到 UARTTask 首次 Stack 采样约为 816 B，首次执行后续 `snprintf()` 调用路径后 High Water Mark 更新为 560 B，并持续稳定。
- 确认 `static char txt_buff[96]` 不位于任务 Stack，因此 816 B → 560 B 的变化来源于实际函数调用路径产生的更深历史 Stack 使用，而不是缓冲区内容变化。
- 最终 baseline 完成 NRST ×3 和完全断电重新上电 ×3，均正常恢复。
- 处理 CubeMX/编辑器导致的大量 CRLF/LF 工作区差异。
- 使用 `.gitattributes`、`git diff --ignore-cr-at-eol` 和逐文件检查确认大量修改仅属于 EOL 差异。
- 清理 FreeRTOS Middleware 和 HAL/CubeMX 生成文件中的 EOL-only 工作区噪音。
- 最终真实功能修改范围缩小为：
  - `STM32F407_Industrial_Platform.ioc`
  - `Src/freertos.c`
- `git diff --cached --check` 通过。
- 创建提交 `9c7333b`：`feat: finalize FreeRTOS scheduling baseline`。
- 提交后工作区 clean，本地功能分支领先远程 1 个 commit，等待 M2 文档收尾后统一 push。

## 验收情况

- `configUSE_PREEMPTION` 配置确认：通过。
- `configUSE_TIME_SLICING` 最终有效值确认：通过。
- 同优先级持续 Ready Task 时间片调度行为：通过。
- 高优先级 UARTTask 抢占行为：通过。
- M2 最终 HeartBeatTask 周期运行：通过。
- M2 最终 UARTTask 周期运行：通过。
- HeartBeatTask Stack 运行时余量：通过，历史最小剩余 384 B。
- UARTTask Stack 运行时余量：通过，历史最小剩余 560 B。
- FreeRTOS Current Free Heap：13592 B，稳定。
- FreeRTOS Minimum Ever Free Heap：13592 B，稳定。
- NRST ×3：通过。
- 完全断电重新上电 ×3：通过。
- M2 最终功能 checkpoint：已创建。
- M2 文档同步和最终远程 push：正在进行。

## 未完成

- 完成 M2 最终项目状态、会话交接、路线图和 README 同步。
- 创建 M2 文档关闭 checkpoint。
- 将最终 M2 功能与文档提交统一 push 到远程。
- Queue、Task Notification、Mutex 等任务间通信与同步留待后续里程碑。
- USART2 RX 尚未进行真实板级验证。
- 尚未进入 ADC、DMA、RS485 或 Modbus 功能阶段。

## 今日 Bug

- 无需要归档到 `ERROR_LOG.md` 的固件 Bug。
- CubeMX/编辑器重新保存工程后，大量 C/H/FreeRTOS 文件在 Git 中显示为 Modified；通过 `git diff --ignore-cr-at-eol` 确认其中绝大多数仅为 CRLF/LF 行尾差异，没有实际代码变化。
- 清理 EOL-only 文件后，最终真实功能修改仅保留 `.ioc` 和 `Src/freertos.c`，避免将无关格式变化写入 Git 历史。

## 今日学习

- `configUSE_TIME_SLICING` 即使没有在 `FreeRTOSConfig.h` 显式定义，也可能由 FreeRTOS 默认配置提供最终有效值，判断配置时需要继续追踪预处理定义。
- 相同优先级、持续 Ready 且不主动阻塞的任务可以在启用时间片时轮流获得 CPU。
- 不同优先级之间的抢占和同优先级之间的时间片调度是两个不同机制。
- FreeRTOS Ready List 不是每次高优先级任务阻塞后都从固定“表头任务”重新开始，调度器会维护同优先级任务的轮换状态。
- Task 被抢占后保存上下文，重新被调度时从保存的执行上下文继续运行，而不是重新执行任务入口。
- `osThreadGetStackSpace()` 反映的是任务历史最小剩余 Stack，而不是简单的瞬时空闲 Stack。
- `static` 局部数组具有静态存储期，不位于任务 Stack。
- `snprintf()` 的目标 buffer 若为 64 B，作为 C 字符串最多保存 63 个有效字符和一个 `'\0'`。
- `HAL_UART_Transmit()` 按指针和长度发送数据，本身不依赖 C 字符串结尾的 `'\0'`。
- `.gitattributes` 和工作区实际 EOL 不一致时可能造成大量无意义 Modified 状态；提交前应区分实际内容变化与 EOL-only 差异。
- `git diff --check` 只检查空白错误，不能判断大量 Modified 文件是否属于合理提交边界。
- 一个高质量 Git checkpoint 不只要求代码正确，还要求提交边界干净、可解释。

## 尚未理解

- PendSV 保存和恢复任务上下文的寄存器级完整过程仍可在后续按需深化。
- 尚未实际使用 Queue、Task Notification、Mutex 等并发通信与同步机制。
- 尚未处理多个任务并发访问同一个日志输出资源的问题。
- USART2 RX 尚未进行实际板级验证。

## Git

- Branch：`feature/m2-freertos-baseline`
- 开发起点：`51f143c`，`feat: add FreeRTOS runtime resource monitoring`
- M2 Final Baseline：`9c7333b`，`feat: finalize FreeRTOS scheduling baseline`
- Remote：`origin/feature/m2-freertos-baseline`
- 当前功能提交尚未 push，等待文档收尾后统一同步。
- 功能提交完成后 Working Tree：clean。

## 明天下一步

完成 M2 最终文档同步并创建 `docs: close M2 and prepare M3 handoff` checkpoint，统一 push `feature/m2-freertos-baseline`。确认本地与远程同步、工作区干净后关闭 M2；下一阶段进入 M3，只建立线程安全、可裁剪的调试基础设施，不提前进入 ADC、DMA、RS485 或 Modbus。

# 2026-09-26

## 今日计划

- 完成 M2 分支进入 main 的最终 Git 收尾。
- 从最新稳定 main 创建 M3 功能分支。
- 检查现有 FreeRTOS UARTTask 中的直接 UART 输出点。
- 开始建立线程安全、可裁剪的最小调试日志模块。

## 实际完成

- 检查 feature/m2-freertos-baseline 的完整提交历史和工作区状态。
- 对 M2 PR 进行 whitespace / EOL 边界检查。
- 保留 FreeRTOS / CMSIS 第三方源码原有 trailing whitespace，不为通过 git diff --check 人为格式化第三方源码。
- 清理 Src/freertos.c 和 Src/stm32f4xx_hal_timebase_tim.c 中两个额外 EOF 空行。
- 创建并推送 `31e15fa：chore: remove extra EOF blank lines`。
- M2 通过 GitHub PR 合并到 main，Merge Commit 为 `72ced0a`。
- 本地 main 成功 fast-forward 到 `72ced0a`，与 origin/main 同步。
- 创建 feature/m3-debug-infrastructure。
- 检查 Src/freertos.c，确认 UARTTask 同时承担日志内容、snprintf() 格式化、HAL_UART_Transmit()、Stack/Heap 监控和错误处理职责。
- 确定 M3 第一阶段只拆分调试日志职责，不提前进入 ADC、DMA、RS485、Modbus、异步日志或正式 IPC。
- 创建 Inc/debug_log.h 与 Src/debug_log.c 初始实现。
- 将 Src/debug_log.c 纳入 CMake 构建。
- 在 FreeRTOS 初始化阶段通过 DebugLog_Init(&huart2) 初始化日志模块。
- 初始日志模块完成 Debug 构建和链接验证。

## 验收情况

- M2 → main PR 合并：通过。
- 本地 main 与 origin/main 同步：通过。
- M3 功能分支创建：通过。
- M3 初始日志模块编译：通过。
- M3 实板日志行为和并发验证：尚未完成。

## 未完成

- 日志模块最终接口整理。
- FreeRTOS UARTTask 迁移。
- 编译期开关与日志等级实测。
- Mutex 并发输出验证。
- Stack / Heap 成本重新测量。
- M3 Final Baseline 稳定性测试。

## 今日 Bug

- Git 从 GitHub 拉取 M2 merge commit 时出现一次 schannel: failed to receive handshake, SSL/TLS connection failed；随后重新执行 git fetch --prune origin 成功，判断为临时网络/TLS 连接问题，没有修改仓库 SSL 安全配置。
- M2 PR 检查期间发现第三方 FreeRTOS / CMSIS 源码自身存在 trailing whitespace；未对第三方源码进行无意义格式化。

## 今日学习

- git diff --check main...feature 只检查两个已提交历史之间的差异，不会包含尚未提交的 Working Tree 修改。
- 第三方中间件源码自身的格式问题与项目自维护源码的格式问题应区别处理。
- 已推送的功能分支不应为了清理两个小问题进行 amend + force push，可以使用独立 housekeeping commit 保留可追踪历史。
- 日志模块应该让 Task 描述“记录什么”，由日志模块统一处理“如何格式化、互斥和发送”。

## 尚未理解

- 尚未通过真实多任务竞争验证日志 Mutex 设计。
- 尚未测量日志模块的真实 Flash、RAM、Task Stack 和 FreeRTOS Heap 成本。

## Git

- M2 Final Branch：feature/m2-freertos-baseline
- M2 merge：72ced0a
- 当前 Branch：feature/m3-debug-infrastructure
- M3 起点：72ced0a
- Working Tree：M3 初始日志实现开发中。

## 明天下一步

- 继续 M3。先完成日志模块接口和编译期裁剪，再逐步迁移 UARTTask 输出；每完成一层先构建和验证，不直接扩大到异步日志或其他业务模块。

# 2026-09-27

## 今日计划

- 继续推进 M3 线程安全、可裁剪调试基础设施。
- 将 FreeRTOS 任务中的直接 UART 调试输出迁移到统一日志接口。
- 验证日志等级过滤、全局编译期开关和多任务并发输出线程安全。
- 测量日志基础设施引入后的 Flash、静态 RAM、Task Stack 和 FreeRTOS Heap 成本。
- 完成 M3 Final Baseline 的 NRST 和完全断电重新上电稳定性验证。

## 实际完成

- 完成 debug_log.h / debug_log.c 日志基础模块整理并建立首个 M3 功能 checkpoint。
- 日志模块使用 USART2 作为当前输出后端，并通过 DebugLog_Init(&huart2) 注入 UART Handle，避免日志模块直接依赖固定的全局 huart2。
- 日志模块使用 128 B 静态共享缓冲区，通过 CMSIS-RTOS2 Mutex 同时保护日志格式化缓冲区和 UART 发送过程。
- 建立 DEBUG / INFO / WARN / ERROR 四级日志接口。
- 建立 DEBUG_LOG_ENABLE 全局编译期开关。
- 建立基于预处理器的日志等级编译期过滤机制。
- Src/freertos.c 中原有两处直接 HAL_UART_Transmit() 调试输出均已迁移到统一 LOG_INFO() 接口。
- UARTTask 当前周期日志为：
  - [I][UART] USART2 TX OK
  - [I][SYS] HeartFree = ... UARTFree = ... Heap = ... MinHeap = ...
- Src/freertos.c 中已经不再保留日志用途的 HAL_UART_Transmit()、snprintf()、txt_buff、tx_len 和 usart2_test_message。
- 创建并推送 `6a760d3：feat: establish debug logging infrastructure`。
- 创建并推送 `4701c62：feat: route FreeRTOS runtime output through debug logger`。
- 完成 DEBUG_LOG_ENABLE = 1 / 0 两组 clean build 对比。
- 完整日志启用时构建结果：
  - RAM：21056 B / 128 KB，16.06%。
  - FLASH：27424 B / 1 MB，2.62%。
  - text：27320 B。
  - data：96 B。
  - bss：20960 B。
- 完整日志关闭时构建结果：
  - RAM：20512 B / 128 KB，15.65%。
  - FLASH：21316 B / 1 MB，2.03%。
  - text：21292 B。
  - data：16 B。
  - bss：20496 B。
- 实测完整日志调用链带来的构建差值约为：
  - FLASH：+6108 B。
  - 静态 RAM：+544 B。
- 完成 DEBUG、INFO、WARN、ERROR 四种日志过滤等级的实板验证，四轮实际输出均与预期一致。
- 完成两个同优先级任务的高频并发日志压力实验：
  - HeartBeatTask 和 UARTTask 均为 osPriorityNormal。
  - 两个任务均约每 10 ms 输出一条长日志。
  - 使用不同 A/B 长字符串和独立计数器扩大共享 UART 竞争窗口。
  - 连续输出期间未观察到半行交叉、A/B 内容混合、乱码、Hang 或 HardFault。
  - 压力实验完成后已删除临时 MTA/MTB 测试逻辑，恢复正常周期任务 Final Baseline。
- M3 Final Baseline 实测资源数据稳定为：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：512 B。
  - FreeRTOS Current Free Heap：13504 B。
  - FreeRTOS Minimum Ever Free Heap：13504 B。
- 相比 M2 Final Baseline：
  - UARTTask 历史峰值 Stack 使用量增加约 48 B。
  - FreeRTOS Heap 额外消耗约 88 B。
  - M3 Final Baseline 完成连续 3 次 NRST 验证，均正常恢复。
  - M3 Final Baseline 完成连续 3 次完全断电重新上电验证，均正常恢复。
  - 六次重新启动后日志、Heartbeat、Stack 和 Heap 监测均正常，未观察到 HardFault、Hang 或 Heap 持续下降。

## 验收情况

- debug_log 独立日志模块：通过。
- UART 日志统一入口：通过。
- DEBUG / INFO / WARN / ERROR 四级日志：通过。
- 编译期日志等级过滤：通过。
- DEBUG_LOG_ENABLE 全局裁剪：通过。
- 多任务共享 UART Mutex 保护：通过当前压力测试验证。
- FreeRTOS 运行时输出迁移：通过。
- 完整日志 Flash / RAM 成本测量：完成。
- Task Stack 重新测量：完成。
- FreeRTOS Heap 重新测量：完成。
- NRST ×3：通过。
- 完全断电重新上电 ×3：通过。
- M3 功能实现和板级验证：完成。
- M3 文档和 Git 最终关闭：正在进行。

## 未完成

- 完成 M3 最终测试记录、项目状态、知识缺口、路线图、决策和会话交接同步。
- 创建 M3 文档关闭 checkpoint。
- 将 M3 分支通过 PR 合并到 main。
- 尚未进入 M4 Queue、Task Notification、Ring Buffer、数据所有权和正式任务间通信。
- 当前日志为同步阻塞 UART 日志，尚未实现异步 LogTask、Queue、DMA 或 ISR-safe 日志。
- USART2 RX、ADC、DMA、RS485 和 Modbus 仍属于后续里程碑。

## 今日 Bug

- 无需要归档到 ERROR_LOG.md 的固件 Bug。
- 日志头文件整理过程中曾出现条件编译边界修改错误，在提交前通过 git diff 检查发现并恢复，没有进入最终功能基线。
- git diff --check 仍可观察到 CRLF 将被规范化为 LF 的提示，但当前实际 diff 未出现整文件 EOL-only 污染。

## 今日学习

- 多个 Task 使用同一个 UART 和静态日志缓冲区时，UART 外设与共享 buffer 都属于共享资源，Mutex 应覆盖“格式化 + 发送”的完整事务，而不仅仅保护 HAL_UART_Transmit()。
- Mutex 保证的是共享资源互斥访问和单条日志完整性，不保证两个同优先级任务按照固定 A/B 顺序执行。
- osMutexAcquire() 的 timeout 使用 Kernel Tick 语义，不能简单按 HAL UART timeout 的毫秒语义理解。
- 预处理器不能直接依赖 C enum 进行 #if 判断，因此日志等级同时保留预处理数值宏和 C enum 值。
- DEBUG_LOG_ENABLE = 0 可以在编译阶段移除日志调用、Mutex 创建和相关日志实现路径。
- vsnprintf() 等格式化功能可能带来明显 Flash 成本；当前完整日志开启后实测 Flash 增加约 6108 B。
- 静态共享日志 buffer 不占用 UARTTask Stack，但 snprintf() / vsnprintf() 等实际调用路径会增加任务历史峰值 Stack 使用量。
- 创建一个 CMSIS-RTOS2 Mutex 会消耗 FreeRTOS Heap；当前 M3 相比 M2 实测 Free Heap 减少约 88 B。
- 编译器/链接器可以通过 section garbage collection 移除没有真实调用者的日志实现，因此资源成本必须在真实调用路径建立后重新测量。
- 日志压力实验应使用较长字符串和较短周期扩大竞争窗口，但实验代码不应长期保留在 Final Baseline。

## 尚未理解

- 尚未深入分析当前约 6108 B Flash 增量中 libc printf / vsnprintf、日志模块和其他链接依赖分别占多少，可在需要优化固件体积时结合 Map 文件继续分析。
- 尚未实现 ISR-safe 日志策略，当前 LOG_xxx() 仅用于正常 Task / Thread 上下文。
- 尚未实现异步日志、DMA 日志或独立 LogTask。
- 尚未系统学习 Mutex 优先级继承、优先级反转及更复杂共享资源场景。
- Queue、Task Notification、Ring Buffer 和数据所有权将在 M4 中继续学习。

## Git

- Branch：feature/m3-debug-infrastructure
- M3 起点：72ced0a，Merge pull request #2 from wwu15768-byte/feature/m2-freertos-baseline
- 日志基础设施 checkpoint：6a760d3，feat: establish debug logging infrastructure
- FreeRTOS 日志迁移 checkpoint：4701c62，feat: route FreeRTOS runtime output through debug logger
- Remote：origin/feature/m3-debug-infrastructure
- 当前功能分支与远程功能 checkpoint 已同步。
- M3 文档关闭 checkpoint：待创建。

## 明天下一步

- 完成 M3 最终文档同步，检查真实 diff 边界并创建 docs: close M3 and prepare M4 handoff checkpoint。推送远程后通过 PR 将 M3 合并到 main；确认本地与远程稳定基线同步后再进入 M4，不提前开始 ADC、DMA、RS485 或 Modbus。

# 2026-09-29

## 今日计划

- 核对仓库实际代码、M0～M3 测试记录和 Git checkpoint。
- 正式扩展项目路线，保持 M0～M3 历史不变并将 M4 设为当前阶段。
- 同步路线相关文档，检查旧里程碑编号、状态冲突和修改边界。

## 实际完成

- 核对 `debug_log`、FreeRTOS 时基/任务代码、TEST_RECORD、DECISIONS 和 Git 历史，确认 M0～M3 均有实际证据支撑。
- 确认 M3 已具备同步 UART Logger、四级 Log Level、错误码、日志宏、`snprintf()` / `vsnprintf()`、CMSIS-RTOS2 Mutex、线程安全输出和编译期裁剪。
- 确认当前没有系统化 HardFault Diagnostic、ADC DMA、UART DMA、CAN、Bootloader 或 Watchdog 实现。
- 将 ROADMAP 从 M0～M12 扩展为 M0～M15：
    - M5 升级为 ADC + Timer + DMA 数据采集子系统。
    - 新增 M10 CAN。
    - 新增 M11 Bootloader / IAP。
    - 新增 M12 Watchdog / Reset Cause / Fault Diagnostics。
    - Old M10 / M11 / M12 顺延为 New M13 / M14 / M15。
- 为 M0～M15 分别补充 Goal、Knowledge、Implementation、Verification 和 Deliverables。
- 同步 README、PROJECT_STATUS、DECISIONS、WEEKLY_PLAN 和 SESSION_HANDOFF，使当前状态统一为“M3 已完成，M4 当前”。
- 明确 Linux 只作为开发与用户态辅助能力，不进入 STM32 项目里程碑，也不加入 Linux Kernel 主线。

## 验收情况

- M0～M3 已完成历史：保留。
- M4 当前状态：已同步。
- M0～M15 五类路线信息完整性检查：通过。
- Old M10 → New M13、Old M11 → New M14、Old M12 → New M15 映射：已记录。
- 源码、CubeMX、`.ioc`、硬件引脚和外设配置：未修改。
- 新功能代码与未来空模块：未创建。
- 固件构建与板级测试：本次未执行，因为没有修改固件；沿用已记录的 M3 Final Baseline 证据，不新增或伪造结果。

## 未完成

- 本次文档修改尚未 commit，等待用户检查。
- M3 分支尚未通过 PR 合并到 `main`。
- M4 Queue、Task Notification、Binary Semaphore、Mutex 对比和 Ring Buffer 尚未开始实现。

## 今日 Bug

- 无固件 Bug。
- 文档检查发现两处 Markdown 行尾空格，已在提交前清理。

## 今日学习

- 路线图中的“待开始”必须描述目标与验收边界，不能写成已经实现。
- 扩展项目范围时应保持已完成历史稳定，通过新增里程碑和编号映射表达变化。
- CAN、Bootloader 和 Watchdog 只有建立在 M4 并发基础、M5 数据链路和 M6～M9 通信/数据模型之上，才不会变成相互割裂的技术堆叠。

## 尚未理解

- M4 各实验的最终消息结构、Queue Length、阻塞时间和 Queue 满策略尚未设计；进入 M4 编码前再结合最小实验确定。
- ADC、CAN、RS485 的具体引脚和硬件资源尚未做冲突检查，不在当前阶段假设。

## Git

- Branch：`feature/m3-debug-infrastructure`。
- M3 文档关闭 checkpoint：`75523b6`。
- 本次路线文档修改：未提交，等待用户检查后决定是否 commit。
- 本次不执行自动 commit。

## 明天下一步

- 用户检查本次文档 diff。确认后再决定 commit，并完成 M3 到 `main` 的 Git 收尾；随后从稳定基线建立 M4 功能分支，先设计和验证最小结构化 Queue 生产者/消费者实验。

# 2026-09-30

## 今日计划

- 正式进入 M4 RTOS 通信与同步阶段。
- 从已完成 M3 的稳定 `main` 基线建立 M4 功能分支。
- 建立最小结构化 Queue Producer / Consumer 实验。
- 验证 Queue 按值传递、阻塞接收、Queue Full、背压、数据丢失和序号缺口行为。
- 测量新增 Queue 和 Task 后的 FreeRTOS Heap 与 Task Stack 使用情况。
- 在压力路径下验证任务 Stack 配置是否安全，并形成 M4 Queue 通信 baseline。

## 实际完成

- 确认 M3 已通过 PR 合并到 `main`，Merge Commit 为 `c13b8e5`。
- 从最新稳定 `main` 建立 `feature/m4-rtos-communication-baseline`。
- 将扩展后的 M0～M15 路线文档提交到 M4 分支。
- 创建并推送 `ed6c3a7：docs: update roadmap for expanded project milestones`。
- 为避免继续扩大 `Src/freertos.c` 的应用逻辑职责，建立独立 M4 IPC 模块：
  - `Inc/m4_ipc.h`
  - `Src/m4_ipc.c`
- 在根目录 `CMakeLists.txt` 中显式加入 `Src/m4_ipc.c`，不修改 CubeMX 生成的 `cmake/stm32cubemx/CMakeLists.txt`。
- `Src/freertos.c` 只负责调用 `M4Ipc_Init()` 启动 M4 IPC 模块，M4 Queue、Task Handle 和具体任务逻辑保持在 `m4_ipc.c` 内部。
- 建立 `M4QueueMessage_t` 结构体，包含：
  - `sequence`
  - `value`
  - `tick`
- 当前 Queue 消息大小为 12 B。
- 建立长度为 8 的 `M4DataQueue`。
- 建立 `M4ProducerTask` 和 `M4ConsumerTask`。
- Queue 正常 baseline 配置：
  - Queue Length：8。
  - Producer Period：200 ms。
  - Producer Priority：`osPriorityNormal`。
  - Consumer Priority：`osPriorityNormal`。
  - Producer Stack：最终调整为 1024 B。
  - Consumer Stack：1024 B。
  - Queue Put Timeout：0。
  - Consumer Queue Get Timeout：`osWaitForever`。
- Producer 使用局部 `M4QueueMessage_t` 生成测试数据，并通过 `osMessageQueuePut()` 将结构体内容按值复制进入 Queue。
- Consumer 使用 `osMessageQueueGet(..., osWaitForever)` 阻塞等待消息，没有数据时进入 Blocked 状态，不进行 Busy Polling。
- 正常路径实板输出连续观察到：
  - `seq=1, 2, 3 ...`
  - `value = sequence × 10`
  - Tick 基本稳定每 200 tick 增加一次。
  - Queue Get 后剩余 `count=0`。
- 正常路径未观察到 sequence gap、Queue Full、Hang 或 HardFault。
- 原 M3 HeartBeatTask、UARTTask 和 Debug Logger 在 M4 Queue 实验期间继续正常工作。
- 首轮 Debug 构建通过，构建结果为：
  - RAM：21072 B / 128 KB，16.08%。
  - FLASH：29252 B / 1 MB，2.79%。
  - text：29148 B。
  - data：96 B。
  - bss：20976 B。
- 初始正常路径资源实测：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：496 B。
  - M4ProducerTask 历史最小剩余 Stack：296 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - FreeRTOS Current Free Heap：11560 B。
  - FreeRTOS Minimum Ever Free Heap：11560 B。
- 相比 M3 Final Baseline 的 13504 B Free Heap，初始 M4 Queue + 2 Task 配置额外消耗约 1944 B FreeRTOS Heap。
- 完成 Queue Full / Backpressure 压力实验：
  - Producer 临时调整为约每 50 ms 生产一条消息。
  - Consumer 临时增加约 500 ms 处理延迟。
  - Producer 速率明显高于 Consumer。
- 压力测试中实物观察到 Queue Count 上升并达到 8。
- Queue 满后，`osMessageQueuePut()` 因 timeout = 0 立即失败，没有阻塞 Producer。
- 建立 `drop_count`，观察到 Drop Count 持续增加。
- 压力实验日志实际观察到：
  - `queue full: drop=1 seq=10 count=8`
  - 后续 Drop Count 持续增加。
- 确认 Queue Full 不会破坏已经成功进入 Queue 的旧消息；Consumer 在 Queue 已经开始产生 Drop 后，仍继续依次消费先前已经排队的连续消息。
- 当旧 Queue 库存被逐步消费后，Consumer 开始观察到 sequence gap。
- 实际观察到包括：
  - `expected=10 actual=12`
  - `expected=13 actual=22`
  - 后续持续出现与 Drop 对应的序号缺口。
- 完成完整背压证据链验证：
  - Producer 长期速度高于 Consumer。
  - Queue Occupancy 上升。
  - Queue 满。
  - Put 失败。
  - Message Drop。
  - Consumer 最终检测到 Sequence Gap。
- 压力测试中发现初始 512 B Producer Stack 的历史最小剩余空间降至 0 B。
- 对比正常路径和压力路径，确认 Stack 峰值主要由 Queue Full 后的 `LOG_WARN()` 异常日志路径进一步放大，而不是正常 `osMessageQueuePut()` 路径本身。
- 将 M4ProducerTask Stack 从 512 B 调整为 1024 B。
- 以相同压力条件重新验证后资源数据稳定为：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：496 B。
  - M4ProducerTask 历史最小剩余 Stack：512 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - FreeRTOS Current Free Heap：11048 B。
  - FreeRTOS Minimum Ever Free Heap：11048 B。
- Producer Stack 从 512 B 增加至 1024 B 后，Free Heap 从 11560 B 降至 11048 B，差值正好为 512 B，与新增 Task Stack 容量一致。
- 确认 1024 B Producer Stack 在当前最坏压力和错误日志路径下仍保留约 512 B 历史最小余量。
- 压力实验结束后恢复 Final Baseline：
  - Producer Period 恢复为 200 ms。
  - 删除 Consumer 人工 500 ms 延迟。
  - Queue Length 保持 8。
  - Producer Stack 保持 1024 B。
  - Consumer Stack 保持 1024 B。
  - Queue Put Timeout 保持 0。
  - Consumer 使用 `osWaitForever` 阻塞等待。
- Final Baseline 完成稳定性验证：
  - Sequence 持续连续。
  - Queue Count 正常回到 0。
  - 未观察到 Queue Full。
  - 未观察到 Sequence Gap。
  - HeartBeatTask 和 UARTTask 正常运行。
  - Heap 和 Stack 保持稳定。
- 创建并推送 `b78282c：feat: establish RTOS queue communication baseline`。
- 提交后本地 `feature/m4-rtos-communication-baseline` 与远程同步。
- 最终 Working Tree：clean。

## 验收情况

- M4 功能分支建立：通过。
- 独立 M4 IPC 模块：通过。
- 结构化 Queue Message：通过。
- Queue Length = 8：通过。
- Producer → Queue → Consumer 最小闭环：通过。
- Queue 按值复制行为：通过。
- Consumer 阻塞等待：通过。
- 正常路径 Sequence 连续性：通过。
- 正常路径 200 ms Producer 周期：通过。
- Queue Full 人工压力验证：通过。
- Queue Put timeout = 0 不阻塞失败行为：通过。
- Drop Counter：通过。
- Sequence Gap 检测：通过。
- Queue 已入队数据在后续 Put Failure 情况下保持完整：通过。
- Producer / Consumer 长期速率失衡导致 Queue 最终溢出：通过。
- Task Stack 运行时测量：完成。
- FreeRTOS Heap 运行时测量：完成。
- 初始 Producer 512 B Stack 压力路径风险：已发现并修正。
- Producer Stack 调整为 1024 B 后压力复测：通过。
- M3 HeartBeat / UART / Debug Logger 基线保持：通过。
- M4.1 Final Baseline 稳定性测试：通过。
- M4.1 Queue 功能 checkpoint：`b78282c`，已创建并推送。

## 未完成

- M4 Task Notification / CMSIS-RTOS2 Thread Flags 实验尚未开始。
- M4 Binary Semaphore 实验尚未开始。
- Mutex 与 Binary Semaphore 的语义和实际行为对比尚未完成。
- Ring Buffer 模块尚未实现。
- 尚未验证 Task / ISR 通信边界。
- M4 整体里程碑尚未关闭。
- ADC、Timer、DMA 数据采集仍属于 M5，不在当前 Queue baseline 中引入。

## 今日 Bug

- 无需要归档到 `ERROR_LOG.md` 的正式固件 Bug。
- Queue 压力测试中发现 M4ProducerTask 初始 512 B Stack 在 Queue Full + `LOG_WARN()` 错误日志路径下历史最小剩余空间降至 0 B，存在 Stack 安全余量不足风险。
- 将 Producer Stack 调整为 1024 B 后，在相同压力条件下历史最小剩余 Stack 稳定为 512 B，问题已在进入 Final Baseline 前解决。
- 压力测试产生的 Producer 50 ms 和 Consumer 500 ms 人工延迟配置已全部移除，没有进入最终稳定 baseline。

## 今日学习

- FreeRTOS Queue 不只是避免多个 Task 同时修改一个共享全局变量，更重要的是建立明确的数据交接和所有权边界。
- `osMessageQueuePut()` 在当前结构体 Queue 中复制的是 `M4QueueMessage_t` 数据内容，而不是保存 Producer 局部变量地址；Producer 后续修改自己的局部对象不会修改已经入队的数据。
- Queue 空时 Consumer 使用 `osWaitForever` 进入 Blocked 状态，可以避免无意义 Busy Polling，让 CPU 执行其他 Ready Task。
- Queue Put timeout = 0 时，Queue 满会立即返回失败，适合显式观察 Backpressure 和 Drop，而不是让 Producer 阻塞等待空位。
- Queue / Buffer 能吸收短时间突发和调度抖动，但不能解决长期 `Producer Throughput > Consumer Throughput` 的吞吐量失衡。
- Queue Full 只会导致新的 Put 失败，不会破坏之前已经成功进入 Queue 的消息。
- Consumer 不一定在第一次 Queue Full 时立即观察到 Sequence Gap，因为 Queue 中可能仍保存着 Drop 发生前已经成功入队的连续旧数据。
- Sequence Gap 通常会在旧库存消费完、Consumer 开始读取 Drop 之后成功进入 Queue 的新数据时出现。
- Queue Length 增大只能延后长期吞吐失衡下的溢出时间，不能从根本上消除溢出。
- Task Stack 必须按最坏执行路径测量，不能只依据正常路径决定大小。
- Queue Full 后的日志格式化、Logger Mutex 和阻塞 UART 发送路径会显著增加 Producer 的历史峰值 Stack 使用。
- `osThreadGetStackSpace()` 反映任务历史最小剩余 Stack，因此压力场景出现 0 B 是重要的 Stack 风险信号。
- FreeRTOS 动态创建 Task 时增加 Stack Size 会直接减少 FreeRTOS Heap；本次 Producer 增加 512 B Stack 后 Free Heap 实测也减少 512 B。
- Debug Logger 本身会对实时实验时序产生一定扰动，因此正式高实时性 Producer 不应依赖高频同步日志作为主要错误处理机制。
- `osDelay()` 当前只用于 M4 软件 Producer 实验，不应替代后续 M5 的 Hardware Timer → ADC → DMA 硬件采集链路。

## 尚未理解

- 尚未实际验证 CMSIS-RTOS2 Thread Flags 与底层 FreeRTOS Task Notification 的对应行为。
- 尚未比较 Queue 和 Task Notification 在“传数据”与“只通知事件”场景中的成本和使用边界。
- 尚未实际验证 Binary Semaphore 的同步语义。
- 尚未完成 Mutex 与 Binary Semaphore 在资源所有权、优先级继承和事件同步方面的系统对比。
- 尚未实现和验证无 RTOS 依赖的 Ring Buffer。
- 尚未实际验证 ISR → Task 的安全通知路径。
- 尚未进入指针消息、共享 Buffer 生命周期和更复杂数据所有权设计。

## Git

- Branch：`feature/m4-rtos-communication-baseline`
- M4 起点：`c13b8e5`，Merge pull request #3 from wwu15768-byte/feature/m3-debug-infrastructure
- M4 路线更新 checkpoint：`ed6c3a7`，`docs: update roadmap for expanded project milestones`
- M4.1 Queue baseline checkpoint：`b78282c`，`feat: establish RTOS queue communication baseline`
- Remote：`origin/feature/m4-rtos-communication-baseline`
- 本地分支与远程分支同步。
- Working Tree：clean。

## 明天下一步

继续 M4，不进入 ADC、DMA 或其他后续外设功能。下一步建立最小 Task Notification / CMSIS-RTOS2 Thread Flags 实验，重点验证“事件通知而非数据传输”的使用场景、阻塞等待行为和与 Queue 的职责边界；完成正常路径和必要边界验证后再进入 Binary Semaphore。

# 2026-10-05

## 今日计划

- 继续 M4 RTOS 通信与同步阶段，不进入 M5。
- 在现有 `m4_ipc` 模块内建立 M4.2 Task Notification / CMSIS-RTOS2 Thread Flags 最小 Task → Task 事件通知实验。
- 验证 Thread Flags 的阻塞等待、事件唤醒和正常 1:1 通知行为。
- 人工构造同一个 Flag bit 连续重复 Set，验证重复事件是否会像 Queue 一样累计。
- 重新测量新增 Task 后的 Task Stack 和 FreeRTOS Heap。
- 恢复正常配置后完成 NRST 和完全断电重新上电稳定性测试。
- M4.2 验证完成后形成独立功能 checkpoint，并更新项目文档。

## 实际完成

- 基于已经完成的 M4.1 Queue baseline 继续扩展 `m4_ipc` 模块，没有新建独立实验工程。
- 保持原有：
  - `M4ProducerTask`
  - `M4ConsumerTask`
  - `M4DataQueue`
- 新增：
  - `StartM4EventProducerTask`
  - `StartM4WorkTask`
- 定义单一事件位 `M4_EVENT_WORK_READY`。
- M4.2 Final Baseline 配置：
  - Event Producer Priority：`osPriorityNormal`。
  - Event Producer Stack：1024 B。
  - Event Work Priority：`osPriorityNormal`。
  - Event Work Stack：1024 B。
  - Event Producer Period：1000 ms。
  - Work 使用 `osThreadFlagsWait(..., osFlagsWaitAny, osWaitForever)` 阻塞等待。
- `StartM4WorkTask` 在没有事件时通过 `osThreadFlagsWait()` 阻塞，不进行 Busy Polling。
- `StartM4EventProducerTask` 周期调用 `osThreadFlagsSet()` 设置 `M4_EVENT_WORK_READY`。
- 正常路径实际观察到：
  - Set Count 从 1 连续增加至 11。
  - Receive Count 从 1 连续增加至 11。
  - 每次返回的 Flag 为 `0x00000001`。
  - 正常路径保持 11 Sets → 11 Receives。
- 原 M4 Queue 在 Thread Flags 实验期间继续正常运行：
  - Sequence 持续连续。
  - Queue Count 正常保持为 0。
  - 未观察到 Queue Full。
  - 未观察到 Sequence Gap。
- 完成重复 Flag bit 合并行为实验。
- 为使实验结果具有确定性，临时将 `StartM4EventProducerTask` Priority 调整为 `osPriorityAboveNormal`，Work 保持 `osPriorityNormal`。
- 每轮 Burst 在 Producer 重新阻塞前连续调用 3 次：
  - `osThreadFlagsSet(M4_EVENT_WORK_READY)`
- 实际观察到：
  - Burst 1：Total Set = 3，Receive Count = 1。
  - Burst 5：Total Set = 15，Receive Count = 5。
  - Burst 10：Total Set = 30，Receive Count = 10。
  - Burst 11：Total Set = 33，Receive Count = 11。
- 确认同一个 Flag bit 在 Work 获得运行机会前被重复 Set 时不会累计成多个独立待处理事件，而是合并为一个 Pending Flag 状态。
- 确认 Thread Flags 适合表达“某项工作待处理”，不能直接作为无损事件计数器使用。
- Burst 压力实验期间资源数据稳定为：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：504 B。
  - M4ProducerTask 历史最小剩余 Stack：808 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - M4EventProducerTask 历史最小剩余 Stack：520 B。
  - M4WorkTask 历史最小剩余 Stack：528 B。
  - FreeRTOS Current Free Heap：8776 B。
  - FreeRTOS Minimum Ever Free Heap：8776 B。
- Burst 实验完成后已经恢复 Final Baseline：
  - Event Producer Priority 恢复为 `osPriorityNormal`。
  - 删除连续 3 次 Set 的 Burst 测试逻辑。
  - 恢复每 1000 ms 单次 Set。
- Final Baseline 稳定资源数据为：
  - HeartBeatTask 历史最小剩余 Stack：384 B。
  - UARTTask 历史最小剩余 Stack：504 B。
  - M4ProducerTask 历史最小剩余 Stack：808 B。
  - M4ConsumerTask 历史最小剩余 Stack：496 B。
  - M4EventProducerTask 历史最小剩余 Stack：528 B。
  - M4WorkTask 历史最小剩余 Stack：528 B。
  - FreeRTOS Current Free Heap：8776 B。
  - FreeRTOS Minimum Ever Free Heap：8776 B。
- 相比 M4.1 Final Baseline 的 11048 B Free Heap，加入两个 1024 B Task 后当前 Free Heap 减少 2272 B，其中包含两个 Task Stack 以及对应 RTOS Task 管理开销。
- Final Baseline 已完成：
  - NRST ×3：全部通过。
  - 完全断电重新上电 ×3：全部通过。
- 六次重新启动后 Stack、Heap、Queue、Thread Flags、Heartbeat、UART 和 Debug Logger 均能够稳定恢复。
- 未观察到 HardFault、Hang、Heap 持续下降、Queue 异常或 Thread Flag 错误。
- 首轮资源日志因为单行过长被 Debug Logger 单条 buffer 截断，后续将系统资源输出拆分为两条日志，没有为此扩大日志 buffer。

## 验收情况

- M4.2 Task → Task Thread Flags 最小链路：通过。
- `osThreadFlagsSet()` 基本通知行为：通过。
- `osThreadFlagsWait(..., osWaitForever)` 阻塞等待：通过。
- Work 事件到达后唤醒：通过。
- 正常路径 11 Sets → 11 Receives：通过。
- 同一个 Flag bit 重复 Set 合并行为：通过。
- Burst 33 Sets → 11 Receives：通过。
- Queue 与 Thread Flags 同时运行：通过。
- Queue 正常 Sequence 连续性：通过。
- 新增 Task Stack 运行时测量：完成。
- FreeRTOS Heap 重新测量：完成。
- Burst 临时优先级与测试代码恢复：完成。
- M4.2 Final Baseline NRST ×3：通过。
- M4.2 Final Baseline Power-cycle ×3：通过。
- 未观察到 HardFault、Hang 或 Heap 持续下降。
- M4.2 板级功能验收：通过。

## 未完成

- M4 Binary Semaphore 实验尚未开始。
- Binary Semaphore 与现有 Debug Logger Mutex 的系统对比尚未完成。
- Ring Buffer 尚未实现。
- 多个不同 Thread Flag bit 的组合行为尚未专门实验。
- ISR → Task Thread Flags / Notification 边界尚未验证。
- M4 整体里程碑尚未关闭。
- ADC、Timer、DMA 仍属于 M5，不在当前 M4.2 中引入。

## 今日 Bug

- 首轮 Thread Flag 正常测试中，Work 日志的 `receive_count` 持续输出为 0。修正计数位置 / 日志参数后，Receive Count 能够从 1 正常连续递增，并与正常 Set Count 保持 1:1。
- 首轮 SYS 资源日志单行过长，超过当前 Debug Logger 单条消息容量，导致 `MinHeap` 尾部被截断。后续将资源日志拆分成两行，完整观察到 `Heap=8776 B` 和 `MinHeap=8776 B`。
- 以上问题均在形成 Final Baseline 前修正，不作为当前 Known Bug，也不单独归档到 `ERROR_LOG.md`。

## 今日学习

- Thread Flags 与 Queue 的核心职责不同：
  - Queue 适合携带结构化数据并保存有限数量的独立消息。
  - Thread Flags 更适合轻量表达“某个事件已经发生 / 某项工作需要处理”。
- `osThreadFlagsWait(..., osWaitForever)` 可以让 Work 在没有事件时进入 Blocked 状态，而不是持续轮询。
- 同一个 Thread Flag bit 被重复 Set 时，本质是反复将同一个状态位置 1，而不是建立多个排队事件。
- 如果三个相同 Flag 在 Work 运行前连续到达，最终可能只产生一次 Work 的有效事件消费。
- 因此 Thread Flags 不能被直接当成无损事件计数器。
- 如果未来 ADC DMA Half Complete 和 Full Complete 需要区分，应至少使用不同事件 bit 或建立更加明确的 Buffer Ownership / Queue 机制，不能简单假设相同 Flag 会保存每一次回调次数。
- 人工提高 Event Producer Priority 可以构造稳定、可重复的事件合并实验，使 Work 在多个 Set 完成前无法先运行。
- Debug Logger 的日志打印顺序只能用于功能观测，不能直接视为 RTOS 内核精确调度轨迹。
- Task Stack 仍然必须以压力路径和异常路径的历史最小剩余空间进行判断。
- 每增加新的 Task，都需要重新测量 FreeRTOS Heap 和 Task Stack，而不能沿用旧里程碑资源数据。

## 尚未理解

- 尚未实际验证 ISR → Task Thread Flags / Notification。
- 尚未实际验证多个不同 Flag bit 同时 Pending 时的处理策略。
- 尚未验证 Binary Semaphore 的 Give / Take、阻塞和唤醒行为。
- 尚未完成 Binary Semaphore 与 Mutex 在事件同步、资源所有权和优先级继承方面的实机对比。
- 尚未设计 M5 ADC DMA Half / Full 事件与采集 Buffer Ownership 的最终方案。
- 尚未实现 Ring Buffer 的满、空、回绕以及 ISR / Task 并发访问模型。

## Git

- Branch：`feature/m4-rtos-communication-baseline`。
- M4.1 Queue 功能 checkpoint：`b78282c`，`feat: establish RTOS queue communication baseline`。
- M4.1 Queue 文档 checkpoint：`22d25bd`，`docs: record M4 queue baseline verification`。
- M4.2 Thread Flags 板级功能已经完成验证。
- M4.2 功能 checkpoint：`<M4.2功能提交哈希>`，`feat: establish RTOS thread flag notification baseline`。
- M4.2 文档修改等待本轮检查后建立独立 docs checkpoint。

## 明天下一步

继续 M4，不进入 M5。

下一步建立 M4.3 Binary Semaphore 最小 Task → Task 同步实验，重点验证 Give / Take、阻塞等待和唤醒行为；随后与 M3 Debug Logger 已经使用的 Mutex 对比“事件同步”与“共享资源所有权保护”的职责差异。

# 2026-10-07

## 今日计划

- 继续 M4 RTOS 通信与同步阶段，不进入 M5。
- 建立 Binary Semaphore 最小 Task → Task 同步实验。
- 验证 Semaphore unavailable 时 Consumer 阻塞、Producer Release 后 Consumer 被唤醒。
- 建立 Mutex 共享数据保护实验，对比 Binary Semaphore 与 Mutex 的职责边界。
- 在 Queue、Thread Flags、Semaphore、Mutex 同时运行条件下完成联合稳定性测试。
- 补充新增同步任务的 Stack High Water Mark 和 FreeRTOS Heap 观测。
- 根据实测 Stack 余量调整任务 Stack，形成稳定同步机制 baseline。
- 完成功能 checkpoint、资源调优 checkpoint 和远程同步。

## 实际完成

- 在已有 M4.1 Queue 和 M4.2 Thread Flags baseline 基础上继续增加 Binary Semaphore。
- 新增 Binary Semaphore：
  - `SyncSem`
  - Maximum Count = 1。
  - Initial Count = 0 / Depleted。
- 新增：
  - `SemProducerTask`
  - `SemConsumerTask`
- `SemConsumerTask` 使用 `osSemaphoreAcquire(..., osWaitForever)` 阻塞等待。
- `SemProducerTask` 每约 2000 ms 调用一次 `osSemaphoreRelease()`。
- 实际日志稳定观察到：

```text
[I][SEM] Consumer Waiting
[I][SYS] Producer Released
[I][SEM] Consumer Acquired
[I][SEM] Consumer Waiting
```

- 已确认：
  - Semaphore 初始不可用时 Consumer 进入阻塞等待。
  - Producer Release 后 Consumer 能够恢复执行。
  - Consumer 每次成功 Acquire 后重新进入 Waiting。
  - 当前实验没有使用 Busy Polling。
- Binary Semaphore 实验期间原 M4 Queue 和 Thread Flags 均继续正常运行。
- Semaphore 功能 checkpoint 已形成：
  - `435ba4f`：`feat: establish RTOS semaphore synchronization baseline`

- CubeMX 重新生成后，`debug_log.c` 从生成的 `cmake/stm32cubemx/CMakeLists.txt` 中被移除。
- 将 `Src/debug_log.c` 显式维护在仓库根目录 `CMakeLists.txt` 中，使用户源码不继续依赖 CubeMX 自动生成的 CMake source list。
- 已形成：
  - `34ddce3`：`build: keep debug logger outside CubeMX generated sources`

- 新增 CMSIS-RTOS2 Mutex：
  - `SharedDataMutex`
- 新增：
  - `MutexTaskA`
  - `MutexTaskB`
- 使用共享变量 `shared_value` 构造 Read-Modify-Write 临界区：
  - MutexTaskA：每次 `+1`
  - MutexTaskB：每次 `+10`
- 两个 Task 均使用：

```text
osMutexAcquire(..., osWaitForever)
→ 读取 shared_value
→ 修改 shared_value
→ osMutexRelease()
```

- 日志放在 Mutex Release 之后，避免同步 UART 日志扩大临界区长度。
- 初始观察到日志打印顺序可能与实际临界区执行顺序不同，例如后执行的 Task 日志可能先输出。
- 为区分“临界区真实执行顺序”和“UART 日志输出顺序”，增加 `shared_op_seq`。
- `shared_op_seq` 在 Mutex 临界区内递增。
- 实际观察到：

```text
[I][M4M] seq=37 A value=198->199
[I][M4M] seq=38 B value=199->209

[I][M4M] seq=39 A value=209->210
[I][M4M] seq=40 B value=210->220

[I][M4M] seq=43 A value=231->232
[I][M4M] seq=44 B value=232->242
```

- 已确认按 `shared_op_seq` 判断时：
  - Operation Sequence 连续。
  - 前一次 `after` 与下一次 `before` 连续。
  - 未观察到两个 Task 基于同一个旧值发生覆盖更新。
  - Mutex 能够正确串行化当前共享数据临界区。
- 已形成 Mutex 功能 checkpoint：
  - `c5bc54a`：`feat: establish RTOS mutex protection baseline`

- Queue、Thread Flags、Binary Semaphore 和 Mutex 已在同一固件中长期联合运行。
- Queue 中实际持续观察到：
  - `sequence` 连续。
  - `value = sequence × 10`。
  - Tick 基本保持约 200 tick 周期。
  - Queue Count 正常保持为 0。
  - 未观察到 Sequence Gap。
- Thread Flags 中实际持续观察到：
  - Set Count 与 Receive Count 持续对应。
  - Flag = `0x00000001`。
- Binary Semaphore 持续观察到：
  - Producer Release。
  - Consumer Acquired。
  - Consumer 重新 Waiting。
- Mutex 持续观察到：
  - `shared_op_seq` 连续。
  - 共享数据更新连续。

- 首次加入四个同步实验 Task 后测得历史最小剩余 Stack：

```text
SemProFree=32 B
SemConFree=32 B
MutexAFree=8 B
MutexBFree=8 B
```

- 判断：
  - Semaphore 两个 Task 的 32 B 历史最小余量过低。
  - Mutex 两个 Task 的 8 B 历史最小余量不可作为稳定 Final Baseline。
  - 虽然功能能够运行，但缺少足够 Stack 安全余量。
- 根据真实 High Water Mark 调整 Stack：
  - SemProducerTask：CubeMX Stack Size 128 → 192，即约 512 B → 768 B。
  - SemConsumerTask：CubeMX Stack Size 128 → 192，即约 512 B → 768 B。
  - MutexTaskA：CubeMX Stack Size 128 → 256，即约 512 B → 1024 B。
  - MutexTaskB：CubeMX Stack Size 128 → 256，即约 512 B → 1024 B。
- 调整后长期运行稳定资源数据：

```text
HeartFree=384 B
UARTFree=488 B
M4ProdFree=808 B
M4ConsFree=496 B
M4EvtFree=528 B
M4WorkFree=528 B

SemProFree=288 B
SemConFree=288 B
MutexAFree=488 B
MutexBFree=520 B

Heap=4568 B
MinHeap=4568 B
```

- Stack 调整后：
  - Semaphore Task 历史最小剩余 Stack 提升至 288 B。
  - MutexTaskA 历史最小剩余 Stack 提升至 488 B。
  - MutexTaskB 历史最小剩余 Stack 提升至 520 B。
- FreeRTOS Free Heap 从调整前约 6104 B 降至 4568 B，减少 1536 B。
- 1536 B Heap 下降与四个 Task 增加的 Stack 总量一致。
- Current Free Heap 与 Minimum Ever Free Heap 长期保持一致，未观察到运行期持续 Heap 消耗。
- 已形成 Stack 调优 checkpoint：
  - `c84fed8`：`fix: increase M4 synchronization task stack margins`
- `c84fed8` 已推送至 `origin/feature/m4-rtos-communication-baseline`。

- 当前最终构建结果：

```text
RAM:   21112 B / 128 KB = 16.11%
FLASH: 33724 B / 1 MB   = 3.22%

text = 33620
data = 96
bss  = 21016
dec  = 54732
```

- 当前长期运行未观察到：
  - HardFault。
  - Hang。
  - 不可解释 Reset。
  - Heap 持续下降。
  - Queue Sequence Gap。
  - Thread Flags 丢失正常周期通知。
  - Semaphore 死锁。
  - Mutex 共享数据覆盖。

## 验收情况

- M4 Binary Semaphore 创建：通过。
- Binary Semaphore 初始 Depleted：通过。
- Consumer `osSemaphoreAcquire(..., osWaitForever)` 阻塞等待：通过。
- Producer `osSemaphoreRelease()` 唤醒 Consumer：通过。
- Semaphore 周期同步运行：通过。
- Queue + Thread Flags + Semaphore 同时运行：通过。
- Mutex 创建：通过。
- Mutex Acquire / Release：通过。
- 两 Task 共享数据互斥访问：通过。
- `shared_op_seq` 临界区真实顺序验证：通过。
- Queue + Thread Flags + Semaphore + Mutex 同时运行：通过。
- 新增同步任务 Stack High Water Mark 检查：完成。
- 初始 Stack 安全余量不足风险：已发现。
- Semaphore / Mutex Task Stack 调整：完成。
- 调整后长期运行 Stack 余量：通过。
- Current Free Heap / Minimum Ever Free Heap 稳定性：通过。
- CMake + Ninja + GNU Arm GCC 构建：通过。
- M4 同步机制阶段当前板级功能验收：通过。

## 未完成

- Ring Buffer 尚未实现。
- Ring Buffer Empty / Full / Wrap-around 尚未验证。
- ISR → Task Queue / Thread Flags / Semaphore 边界尚未验证。
- ISR-safe 通信 API 使用边界尚未完成实机验证。
- 当前 Semaphore / Mutex 测试 Task 由 CubeMX 生成在 `freertos.c` 中，与 `DEC-20260930-01` 中“M4 IPC 逻辑集中在 `m4_ipc` 模块”的既定模块边界存在差异，M4 最终关闭前需要决定是否迁移 / 收敛。
- M4 整体里程碑尚未关闭。
- ADC + Timer + DMA 属于 M5，当前仍未进入。

## 今日 Bug

- 无需要作为当前 Known Bug 保留的固件功能错误。
- 首次增加 Semaphore / Mutex Task 后发现：
  - `SemProFree=32 B`
  - `SemConFree=32 B`
  - `MutexAFree=8 B`
  - `MutexBFree=8 B`
- 该问题属于运行时资源安全余量不足，而不是已经发生的 Stack Overflow。
- 已通过扩大四个同步 Task Stack 并重新长期运行验证解决。
- CubeMX Generate Code 再次产生大量 CRLF / LF EOL-only 工作区噪音。
- 使用 `git diff --ignore-space-at-eol` 区分真实代码变化与纯行尾变化，并恢复无实际内容变化的文件。
- 当前功能提交最终均保持修改边界干净。
- `Src/freertos.c` 曾出现 EOF 多余空行，已在推送前修正并通过 `git diff --cached --check`。

## 今日学习

- Binary Semaphore 适合表达“某个同步条件已经发生 / 某个任务可以继续”，不是共享资源所有权锁。
- Binary Semaphore 初始 Count = 0 时，Consumer 可以直接阻塞等待，不需要 Busy Polling。
- Mutex 主要解决共享资源所有权与互斥访问问题，与 Binary Semaphore 的事件同步职责不同。
- Mutex 临界区应该尽量短，不应在持锁期间执行不必要的阻塞日志、Delay 或复杂处理。
- `LOG_INFO()` 放在 Mutex Release 后能够缩短临界区，但日志实际打印顺序可能与临界区执行顺序不同。
- UART 日志顺序不是 RTOS 精确调度 Trace。
- 对并发顺序需要独立的、在临界区内生成的 `shared_op_seq` 作为观测证据。
- Task “能够运行”不等于 Stack 配置安全；必须根据历史最小剩余 Stack 判断安全余量。
- 8 B / 32 B 的历史最小剩余空间虽然没有立即产生 Overflow，但不具备合理工程余量。
- 增大动态创建 Task 的 Stack 会直接减少 FreeRTOS Heap。
- 本次 Stack 总增加量约 1536 B，Free Heap 也实际减少约 1536 B，与资源模型一致。
- Current Free Heap 与 Minimum Ever Free Heap 长期相同，可以作为当前运行期间未出现额外持续动态内存消耗的证据之一。
- CubeMX 自动生成文件与仓库 LF 规则仍存在行尾格式摩擦，功能提交前必须继续检查 EOL-only diff。
- Queue、Thread Flags、Binary Semaphore 和 Mutex 的职责可以初步区分为：
  - Queue：传递并保存独立数据项。
  - Thread Flags：表达轻量 Pending Event State。
  - Binary Semaphore：任务间同步信号。
  - Mutex：共享资源互斥与所有权保护。

## 尚未理解

- 尚未实际验证 Counting Semaphore。
- 尚未构造并验证 Mutex Priority Inheritance / Priority Inversion 场景。
- 尚未验证 ISR → Task Binary Semaphore / Thread Flags / Queue 的具体安全调用边界。
- 尚未验证 Ring Buffer Empty / Full / Wrap-around。
- 尚未建立 ISR 与 Task 之间的 Ring Buffer 并发访问模型。
- 尚未决定 M5 ADC DMA Half / Full 事件最终使用 Thread Flags、Queue、Semaphore 或组合机制。
- 尚未完成 DMA Buffer Ownership 最终设计。

## Git

- Branch：`feature/m4-rtos-communication-baseline`
- Queue 功能 checkpoint：
  - `b78282c`：`feat: establish RTOS queue communication baseline`
- Queue 文档 checkpoint：
  - `22d25bd`：`docs: record M4 queue baseline verification`
- Thread Flags 功能 checkpoint：
  - `fd80cc9`：`feat: establish RTOS thread flag notification baseline`
- Thread Flags 文档 checkpoint：
  - `1008734`：`docs: record M4 thread flag baseline verification`
- CMake 用户源码边界修复：
  - `34ddce3`：`build: keep debug logger outside CubeMX generated sources`
- Binary Semaphore 功能 checkpoint：
  - `435ba4f`：`feat: establish RTOS semaphore synchronization baseline`
- Mutex 功能 checkpoint：
  - `c5bc54a`：`feat: establish RTOS mutex protection baseline`
- 同步任务 Stack 调优 checkpoint：
  - `c84fed8`：`fix: increase M4 synchronization task stack margins`
- Remote：`origin/feature/m4-rtos-communication-baseline`
- 本地与远程已同步。
- Working Tree：clean。

## 明天下一步

继续 M4，不进入 M5。

下一步优先实现独立 Ring Buffer，验证 Empty、Full、Wrap-around、Read / Write Index 和基础数据完整性。Ring Buffer 正常路径和边界通过后，再建立最小 ISR → Task 通信实验，明确 ISR-safe API、非阻塞边界和任务唤醒行为。

同时在 M4 最终关闭前检查 Semaphore / Mutex 实验代码与 `DEC-20260930-01` 的 `m4_ipc` 模块边界是否需要统一。


# 2026-10-16

## 今日计划

- 整理 2026-10-13 至 2026-10-16 的 M4 Ring Buffer 开发成果。
- 完成独立 Ring Buffer 基础模块。
- 验证 Empty、Full、Wrap-around 和 FIFO 数据顺序。
- 验证 Full 时拒绝写入和溢出计数。
- 使用已有 FreeRTOS Task 建立确定性板级测试。
- 检查 Queue、Thread Flags、Mutex 和 UART 的运行情况。
- 检查 Task Stack High Water Mark 和 FreeRTOS Heap。
- 完成功能代码检查、构建检查、Git 提交和远程推送。
- 准备 M4.5 ISR → Task 通信实验。

## 实际完成

- 完成独立 Ring Buffer 模块：
  - `Inc/ring_buffer.h`
  - `Src/ring_buffer.c`
- 通过根目录 `CMakeLists.txt` 将 Ring Buffer 加入固件构建。
- 使用静态固定容量数组，不使用动态内存。
- Ring Buffer 当前物理数组长度为 8 B。
- 使用预留一个空槽的设计，实际有效容量为 7 B。
- 使用 `head` 记录下一写入索引。
- 使用 `tail` 记录下一读取索引。
- Empty 判断：

```c
head == tail
```

- Full 判断：

```c
(head + 1U) % RING_BUFF_SIZE == tail
```

- 已实现：
  - `RingBuffer_Init()`。
  - `RingBuffer_WriteByte()`。
  - `RingBuffer_ReadByte()`。
  - `RingBuffer_IsEmpty()`。
  - `RingBuffer_IsFull()`。
- 已实现满时拒绝写入。
- 已实现 `overflow_count` 累计溢出统计。
- 已明确 Empty 时读取失败不修改输出变量。
- 已明确 Full 时写入失败不移动读写索引。
- Ring Buffer 核心代码不依赖 HAL / FreeRTOS。

- 开发过程中修正：
  - 将 head / tail 统一为数组索引，不再与指针混用。
  - 修正结构体指针空值检查。
  - 修正写入和读取时的数据访问方式。
  - 增加索引循环回绕。
  - 统一头文件与源文件中的宏名称。
  - 统一 RingBufferStatus_t 错误码。
  - 修正 `uint8_t *` 输出参数的使用。
  - 消费者改为只在读取成功时输出数据。
  - 不再依据历史 overflow_count 决定额外读取次数。

- 临时复用：
  - `SemProducerTask`
  - `SemConsumerTask`
- 使用 CMSIS-RTOS2 Thread Flags 控制测试阶段。
- 依次完成：
  - Empty。
  - Full。
  - Full Read。
  - Wrap Write。
  - Wrap Read。
- 实际输出：

```text
[I][RBTEST] EMPTY PASS
[I][RBTEST] FULL PASS, overflow=1
[I][RBTEST] FULL READ PASS
[I][RBTEST] WRAP WRITE PASS, head=1 tail=2
[I][RBTEST] WRAP READ PASS, head=1 tail=1
[I][RBTEST] Consumer test finished
[I][RBTEST] Producer test finished
```

- 已确认：
  - Full 时能够拒绝新数据。
  - 溢出计数能够从 0 增加到 1。
  - Full 后能够正确读取已有数据。
  - Write Index 能够由 7 回绕至 0。
  - Read Index 能够正确回绕。
  - FIFO 数据顺序正确。
  - 最终 head 和 tail 均为 1。
  - 最终 Ring Buffer 恢复 Empty。

- 同一固件中继续观察到：
  - Queue Sequence 连续增长至 46。
  - Queue Count 正常。
  - Thread Flags Set / Receive 计数对应。
  - Mutex 共享数据更新连续。
  - UART 周期输出正常。

- 当前板级资源：

```text
HeartFree=384 B
UARTFree=488 B
M4ProdFree=808 B
M4ConsFree=496 B
M4EvtFree=504 B
M4WorkFree=528 B

SemProFree=248 B
SemConFree=256 B
MutexAFree=520 B
MutexBFree=520 B

Heap=4568 B
MinHeap=4568 B
```

- 当前测试日志约覆盖 9 秒。
- 未观察到 HardFault、Hang 或 Heap 持续下降。
- 本轮未执行完整 NRST / Power-cycle 验证。
- 本轮未验证真正的 ISR → Task 并发访问。

## 验收情况

- Ring Buffer 独立模块建立：完成。
- Ring Buffer 初始化：通过。
- Ring Buffer Empty：PASS。
- Ring Buffer Full：PASS。
- Full 后原有数据保留：PASS。
- Ring Buffer Write Index Wrap-around：PASS。
- Ring Buffer Read Index Wrap-around：PASS。
- FIFO 数据顺序：PASS。
- Full 写入失败和 overflow_count：PASS。
- Empty 读取失败与输出数据保护：PASS。
- Ring Buffer 确定性板级测试：通过。
- CMake + Ninja + GNU Arm GCC 构建：提交前检查通过。
- Queue、Thread Flags、Mutex、UART 短时联合运行观察：通过。
- FreeRTOS Heap 运行时观察：完成。
- Git 功能 checkpoint：完成。
- Git 远程同步：完成。
- Ring Buffer 基础功能验收：通过。
- Ring Buffer 并发安全：未验收。
- M4 Final Baseline：未验收。

## 未完成

- 当前两个 Semaphore Task 被临时用于 Ring Buffer 测试。
- 原 Binary Semaphore 功能尚未在最新测试固件中重新回归。
- 需要恢复或隔离临时测试入口。
- ISR → Task 通信尚未实现和验证。
- ISR-safe API、非阻塞边界尚未实机验证。
- Ring Buffer SPSC 并发安全尚未验证。
- ISR 与 Task 之间的索引所有权和内存可见性尚未验证。
- M4 最终联合稳定性和复位恢复验证尚未完成。
- Semaphore / Mutex 实验代码与 `m4_ipc` 模块边界仍需整理。
- M4 整体尚未关闭。
- M5 ADC + Timer + DMA 尚未开始。

## 今日 Bug

- 本轮开发中曾出现：
  - 结构体指针与数组索引混用。
  - Ring Buffer 宏名称和错误码不一致。
  - ReadByte 输出参数类型不匹配。
  - 未检查读取返回值导致重复输出旧数据。
  - 使用累计 overflow_count 控制额外读取次数。
- 已通过修正实现和返回值判断解决。
- Ring Buffer 确定性测试全部 PASS。
- 当前没有通过本次日志确认的新运行时固件故障。
- 这些已修复问题不作为当前未解决 Known Bug。

## 今日学习

- 理解使用 head / tail 构建 Ring Buffer 的基本原理。
- 理解预留一个空槽区分 Empty / Full。
- 理解物理容量和有效容量的区别。
- 理解索引取模实现循环回绕。
- 理解失败操作不应破坏已有数据和状态。
- 理解 C 指针、结构体指针和数组索引的区别。
- 理解函数返回状态需要由调用者检查。
- 理解日志输出不等于操作成功。
- 理解数据丢失统计必须具有明确语义。
- 理解缓冲区容量只能吸收短期突发。
- 理解长期生产速度高于消费速度会持续积压。
- 理解使用 Thread Flags 串行测试阶段可以提高结果确定性。
- 理解 Ring Buffer 功能测试通过不等于并发安全通过。

## 尚未理解

- Cortex-M4 上 ISR / Task 共享索引的完整内存访问约束。
- `volatile`、原子性和内存顺序的具体区别。
- SPSC Ring Buffer 的严格并发所有权设计。
- 当 Producer 覆盖旧数据时如何避免与 Consumer 竞争 tail。
- CMSIS-RTOS2 不同 API 在 ISR context 下的使用限制。
- ISR → Task 最小通知链路的具体实现。
- DMA Half / Full 事件与数据 Buffer Ownership 的关系。
- 工业数据丢失告警、高水位和恢复机制的正式设计。

## Git

- Branch：`feature/m4-rtos-communication-baseline`。
- 功能 checkpoint：
  - `f8c3e52`：`feat: establish M4 ring buffer baseline`
- 前一个文档 checkpoint：
  - `5c9efac`：`docs: record M4 synchronization baseline verification`
- 功能提交包含：
  - `CMakeLists.txt`
  - `Inc/ring_buffer.h`
  - `Src/ring_buffer.c`
  - `Src/freertos.c`
- 提交前已执行 Git 暂存区检查。
- 功能代码已提交本地仓库。
- 已成功执行 `git push`。
- 远程分支：`origin/feature/m4-rtos-communication-baseline`。
- 功能提交已推送至远程。
- 文档同步修改将另行形成 checkpoint。

## 明天下一步

继续 M4，不进入 M5。

首先完成 Ring Buffer 文档同步和独立文档 Git checkpoint。

随后恢复或隔离临时 Ring Buffer 测试任务，确保原 Semaphore 功能可以重新验证。

下一项核心任务为 M4.5 ISR → Task 通信：

1. 选择一个最小、可控的中断事件源。
2. 核对 CMSIS-RTOS2 ISR-safe API。
3. 建立 ISR 通知 Task 的最小链路。
4. 保证 ISR 不执行同步 UART 日志或阻塞操作。
5. 验证 Task 阻塞、唤醒和事件处理顺序。
6. 再研究 Ring Buffer SPSC 并发数据交接。
7. 重新测量 Stack 和 FreeRTOS Heap。

完成 ISR → Task 后再开展 M4 最终模块整理、联合稳定性和复位恢复验证。


# 2026-10-18

## 今日计划

- 恢复在 M4.4 确定性 Ring Buffer 测试中被临时复用的 Semaphore Task。
- 复核原 Queue、Thread Flags、Mutex、UART 运行。
- 选择 M4.5 TIM7 中断源并完成 CubeMX 配置。
- 处理 CubeMX 重新生成代码导致的 Git CRLF/LF 噪音。
- 为 ISR → Task 通信建立独立应用模块接口。

## 实际完成

- 通过编译开关保留 Ring Buffer 基础测试代码，恢复 `SyncSem` 的 Producer Release / Consumer Acquire 路径。
- 修正 Producer / Consumer 的职责和任务返回问题，保持任务循环运行。
- 完成 Semaphore 板级回归以及原 M4 Queue、Thread Flags、Mutex 和 UART 的运行观察。
- 形成并推送 `c4c0721`：`fix: restore M4 semaphore task baseline`。
- 在 CubeMX 中启用 TIM7，使用 84 MHz timer clock、PSC=8399、ARR=999，目标中断周期 100 ms。
- `main.c` 加入 `MX_TIM7_Init()`；HAL TIM7 MSP 初始化中配置时钟与 NVIC Priority=5；`TIM7_IRQHandler()` 通过 `HAL_TIM_IRQHandler(&htim7)` 进入 HAL。
- 核对 `HAL_TIM_PeriodElapsedCallback()` 中 TIM6 的 `HAL_IncTick()` 条件分支，TIM7 不重复增加 HAL Tick。
- 检查发现 `.ioc` 已保存 `TIM7.Period=999`，而 `tim.c` 曾仍为 `Period=9`；重新生成后最终确认为 999。
- CubeMX 多次生成后出现大量 CRLF/LF 工作区噪音，通过 Git EOL 属性检查、备份、字节级定向 LF 规范化和实际 Diff 审核解决。

## 验收情况

- Semaphore Release / Acquire 连续唤醒：PASS。
- Queue、Mutex、Task Flags、UART 短时回归：PASS。
- Semaphore 恢复阶段运行时 Free Heap / MinHeap：4568 B / 4568 B。
- Semaphore Producer/Consumer 剩余 Stack：288 B / 288 B。
- TIM7 生成参数最终对照：PSC=8399、ARR=999，PASS。
- TIM6 与 TIM7 回调职责源码审查：PASS。
- 本阶段仅完成 TIM7 底层配置/代码接入，不将其写成 ISR → Task 板级验收完成。

## 未完成

- ISR → Task Worker、实际板级通知和高负载事件合并实验（随后阶段推进）。
- M4 Final Integration、最终复位/断电重启。

## 今日 Bug

- CubeMX 生成 CRLF 造成几十个已跟踪文件显示修改；通过定向恢复 LF 解决。
- TIM7 `.ioc` / `tim.c` ARR 曾不一致：9 对应 1 ms，999 对应 100 ms；重新生成后修正。

## 今日学习

- 进一步理解 C/CMSIS Task 返回与生命周期管理。
- 理解 TIM6 HAL Tick 与 TIM7 用户中断应按 Instance 分开。
- 理解 NVIC Priority 5 与 FreeRTOS ISR-safe API 阈值的关系。
- 理解 Git `i/lf w/crlf attr/text eol=lf` 的实际含义，以及 Git Diff 与功能差异的区别。

## 尚未理解

- TIM7 高负载下 ISR/Task 时序与通知合并的定量边界。
- Ring Buffer SPSC 数据所有权、内存可见性。

## Git

- Branch：`feature/m4-rtos-communication-baseline`。
- Semaphore 恢复 checkpoint：`c4c0721`，用户确认本地与 origin 同步、工作树 clean。
- TIM7 与 ISR Worker 后续改动在独立功能提交中形成 checkpoint，不在恢复提交中混入。

## 明天下一步

实现独立 `m4_isr`，检查枚举状态返回、Worker Task 启动 TIM7、Thread Flags 阻塞唤醒，完成 Clean Build 和首次板级验证。

# 2026-10-20

## 今日计划

- 以 TIM7 100 ms 更新中断建立 `ISR → Thread Flags → Worker Task` 通信基线。
- 清晰区分 ISR 非阻塞通知与 Task Context 的日志/处理。
- 测量 ISR Worker 运行时 Stack、FreeRTOS Heap，验证原 M4 模块共存。
- 完成功能提交并同步远程。

## 实际完成

- 建立 `Inc/m4_isr.h`、`Src/m4_isr.c` 独立模块，通过根目录 CMake 加入构建。
- 模块初始化使用枚举返回码；Worker Stack 查询函数使用数值返回 `uint32_t M4ISR_GetWorkerStackFree(void)`。
- Worker 使用 `osThreadFlagsWait(..., osWaitForever)`，由任务启动 `HAL_TIM_Base_Start_IT(&htim7)`。
- 仅在 `main.c` 已有 HAL PeriodElapsed Callback 的 TIM7 分支转发通知，保留 TIM6 HAL 时基逻辑。
- `FreeRTOSConfig.h` 核对 `configPRIO_BITS=4`、`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5`；TIM7 Priority=5，NVIC Grouping 4-bit。
- 实测 `irq=30/40/50/60` 与 `received` 对应，`errors=0`。
- Queue 序号连续至当前截取日志中的 31，Semaphore 持续唤醒，原 Task Flags 与 Mutex 计数正常，UART 周期输出。
- 用户随后确认持续运行测试均通过；完整结束时的实际计数和准确时长未在本次文档输入中提供。
- 最新已确认 Git HEAD：`adf5d5e`：`feat: establish M4 ISR to task notification baseline`，本地与 origin 同步。

## 验收情况

- Debug Clean Build：PASS（FLASH 34748 B，RAM 21192 B）。
- ISR → Task 基础通知：PASS。
- `errors=0`，低负载采样点 `irq=received`：PASS。
- M4 原 Queue、Semaphore、Thread Flags、Mutex、UART 回归观察：PASS。
- Worker Stack 最小剩余：520 B。
- FreeRTOS Current / Minimum Ever Free Heap：3432 B / 3432 B。
- M4.5 持续运行：用户确认通过；不补写缺失的最终统计数据。
- M4 Final Integration：未验收。

## 未完成

- Ring Buffer 真实 SPSC 并发访问和内存所有权验证。
- M4 最终模块归并、最终整体长时间稳定性、NRST ×3 与断电重启 ×3。
- M4.5 独立 docs checkpoint（功能提交已经完成）。

## 今日 Bug

- 本次提供的 M4.5 板级日志未出现新 HardFault、Hang 或通知错误。
- 历史 CubeMX EOL 问题已按规范处理，未在功能提交中保留无关大范围格式化差异。

## 今日学习

- 理解 ISR-safe `osThreadFlagsSet()` 与 Task-only `osThreadFlagsWait()` 的上下文差异。
- 理解同一 Flag bit 的 Pending State 与事件总次数不是同一语义。
- 理解新增 1024 B Task Stack 会消耗 FreeRTOS 动态 Heap，而 ELF 静态 RAM 不等于运行时 Heap。
- 理解初始化阶段成功与 Worker 内稍后启动 TIM7 成功不是同一个返回状态。

## 尚未理解

- 高负载下的事件合并、通知延迟和吞吐量界限。
- 用 SPSC Ring Buffer 安全地承载真实 ISR 数据。
- M5 DMA Half / Full Buffer Ownership 模型。

## Git

- 功能 checkpoint：`adf5d5e`，`feat: establish M4 ISR to task notification baseline`。
- Parent：`c4c0721`；分支与远程状态由用户确认一致，工作树 clean。
- 文档更新与 M4 Final Integration 后续单独提交。

## 明天下一步

以 M4.5 完整测试记录为基础同步项目文档；之后进入 M4 Final Integration，明确当前正式功能和未验证边界。

# 2026-10-23

## 今日计划

- 按照已有 `docs.zip` 的九份文档原格式同步目前已验证的 M4.5 状态。
- 统一 Git HEAD、测试结论、里程碑位置与交接任务。
- 保留旧历史，注明 Ring Buffer 字节容量的资料差异。

## 实际完成

- 本次交付为新的文档 ZIP；将现有九份 Markdown 内容基于对话已确认事实同步到 2026-10-23。
- 未修改用户本地工程源码；未代替用户执行本地 Git 提交或推送。

## 验收情况

- M4.5 功能提交 `adf5d5e` 由用户确认已在 origin 分支，工作区 clean（文档更新前）。
- 本次文档包的文件完整性、章节结构和相互引用在生成时检查；导入本地后仍需人工复核 Diff。

## 未完成

- 将文档 ZIP 内容合并回本地仓库。
- 文档独立 Git 提交与推送。
- M4 Final Integration 与最终 M4 关闭。

## 今日 Bug

- 暂无新增固件 Bug 报告；文档中识别到 Ring Buffer 旧叙述与此前源码快照的数据类型差异，待对照最新仓库核实。

## 今日学习

- 区分经实测证明的功能、用户确认通过的测试和未提供原始结尾数据的边界。
- 单一职责文档需要同步当前状态，但不应覆盖旧日期的原始测试事实。

## 尚未理解

- Ring Buffer 最新 HEAD 中实际元素类型与 API 命名是否已调整；当前仅有先前源码快照。

## Git

- 已确认功能 HEAD：`adf5d5e`（此 ZIP 准备前）。
- 新的 docs checkpoint：尚未产生。

## 明天下一步

审核导入后的 Markdown Diff，独立提交和推送文档；再开展 M4 最终联合稳定性与 NRST / Power-cycle 验收。


# 2026-10-27

## 今日计划

- 在 M4 关闭前补足 Ring Buffer 真正的 Task ↔ Task SPSC 并发安全测试，并与现有 M4 模块联合运行。
- 确认新增测试模块独立、可通过 CMake 开关关闭；保留已验证的通信基线。
- 对并发测试结果、运行时资源和 Git checkpoint 形成可追溯记录。

## 实际完成

- 在干净工作区基础上创建 `feature/m4-spsc-concurrency-verification`，与原 `feature/m4-rtos-communication-baseline` 隔离。
- 修改 `Inc/ring_buffer.h`、`Src/ring_buffer.c`：采用 `uint32_t` 元素、8 槽中保留 1 个空槽，并使用 C11 `_Atomic` 索引及 acquire/release 内存序构建 SPSC 发布/消费关系。当前测试模式限定单生产者和单消费者。
- 新增 `Inc/m4_spsc_test.h`、`Src/m4_spsc_test.c`，使用独立的 SPSC Producer / Consumer 测试任务；通过根 `CMakeLists.txt` 中的 `M4_ENABLE_SPSC_TEST` 可选启用，不占用 Semaphore Task 的职责。
- 在 `Src/freertos.c` USER CODE 区域接入测试模块和 SPSC 任务 Stack 监测；测试开关默认为 OFF（正式构建前仍应显式确认 CMake Cache 为 OFF）。
- 实际板级日志：`[SPSC] PASS sent=50000 recv=50000 full=7064 empty=7419`；`[SPSC] overflow=7064 elapsedTicks=7501`。
- 同一启动窗口中，Queue `seq=1..40` 连续、Semaphore Release / Acquire 连续、Mutex 共享数值正确、Task Flags 对应、TIM7 ISR 已展示至 `irq=70 received=70 errors=0`。
- 资源采样：`Heap=3432 B`、`MinHeap=3432 B`、`SpscPFree=876 B`、`SpscCFree=444 B`、`ISRWorkerFree=520 B`。测试任务的存在没有在展示的 SYS 采样中引起 FreeRTOS 动态 Heap 下降。
- 用户提供分支 Git 记录：`d93665e (HEAD, origin/feature/m4-spsc-concurrency-verification)`，功能提交 `feat: verify M4 ring buffer SPSC concurrency baseline`；用户确认工作区 clean。
- 在先前的 M4 联合验证中，使用临时 `freertos.c` 先做 Ring Buffer EMPTY / FULL / WRAP 确定性测试，再进入 Semaphore 循环，用户确认长期运行通过；该临时实验不等于正式长期保留架构。

## 验收情况

- SPSC Task ↔ Task 50,000 个连续元素的生产、消费和顺序校验：**PASS**（专项测试程序汇总）。
- 满缓冲重试 `7064` 次，与 `overflow_count=7064` 一致；空缓冲重试 `7419` 次。重试不代表最终丢样，测试成功接收数仍为 `50000`。
- 测试区间的 Stack / Heap 监测和其他 M4 模块短时联合运行：**PASS（提供的日志窗口）**。
- `git diff --check` 无输出（提交前由用户提供）；功能提交已推送到独立远程分支。
- 单次测试耗时 `7501 ticks`，按本项目 1 kHz OS tick 配置约 7.501 s；不据此推导业务实时性/最坏延迟。

## 未完成

- 尚未得到本轮 SPSC 专项的 NRST ×3、完全断电上电 ×3 的单独证据。
- 尚未得到明确的 SPSC OFF 后正式固件 Clean Build + 烧录回归结果；需验证 CMake Cache 不残留 `ON`。
- 尚未确认 SPSC 分支已通过 PR 合并到 `feature/m4-rtos-communication-baseline`，也没有证据显示 M4 已合并到 `main`。
- 最终 M4 里程碑关闭文档、合并后最终固件回归及 M5 起始分支仍待完成。

## 今日 Bug

- 本轮 SPSC 板级日志未报告新增固件 Bug；历史 Ring Buffer 文档误将有效 7 个 `uint32_t` 元素写成 7 B 的问题，已确定为旧资料口径错误，需以新决策补充，而不覆盖旧测试原文。

## 今日学习

- 分阶段 Thread Flags 交替读写验证的是基础功能；真正 SPSC 必须让生产者和消费者独立调度并验证顺序/完整性。
- Full 时不覆盖已发布元素、重试同一数据，可以验证背压下的数据完整性；`overflow_count` 表达失败写入尝试次数，并不自动等于业务样本丢失数。
- C11 acquire/release 约束已发布数据的内存可见性；只验证本次 Task ↔ Task SPSC，不推断 ISR ↔ Task、DMA Ownership 或 MPSC 安全。

## 尚未理解

- 高负载与不同优化条件下的最坏延迟、吞吐量上界、负载下积压预算；ISR/DMA 与任务间的数据所有权仍需另行设计。

## Git

- SPSC Branch：`feature/m4-spsc-concurrency-verification`。
- SPSC HEAD / origin：`d93665e`，工作区由用户确认 clean。
- 父级 M4 Branch：`feature/m4-rtos-communication-baseline`，已知 HEAD 为 `405b9ab`（`docs: record M4 ISR notification baseline verification`）。
- 本 ZIP 的 2026-10-27 文档更新尚未在本地仓库执行 Git 提交；PR 合并与 M4 关闭均不可提前标记成功。

## 明天下一步

办理 M4 → main 合并与 M5 准备。


# 2026-10-28

## 今日计划

- 在 M4 已合并的 M5 分支上建立任务与串口清理的可回退基线；让正式运行态不再长期创建 M4 教学实验任务。
- 删除无业务用途的演示任务、同步对象与关联变量；保留有复用价值的 Ring Buffer、统一 Logger，以及经过整理的 IPC / ISR 接口。
- 处理 CubeMX 重新生成带来的 CRLF/LF 工作区噪音；完成构建、30 秒串口与资源对比、复位和断电重启回归。

## 实际完成

- 根据用户给出的 Git 输出，`main`、`origin/main` 和 `feature/m5-adc-timer-dma-baseline` 的清理前起点均为 `e793039`（M4 PR #5 Merge）；SPSC → M4 的 PR #4 Merge 为 `ccbd176`，M4 文档关闭提交为 `3a8fa13`。此前检查的工程归档含 `m5-pre-cleanup-20261028` 标签；本次未重新查询远端标签。
- 修改 `STM32F407_Industrial_Platform.ioc`：移除 `SemProducerTask`、`SemConsumerTask`、`MutexTaskA`、`MutexTaskB`、`SyncSem` 和 `SharedDataMutex`；默认应用任务仅保留 `HeartBeatTask` 与 `UARTTask`。
- 修改 `Src/freertos.c`：清理原实验任务/变量、移除 `M4Ipc_Init()` 和 `M4ISR_Init()` 的默认调用，保留 `M4_ENABLE_SPSC_TEST` 条件初始化入口；按每 10 次 UART 循环输出一次 Stack/Heap 监控及 UART 状态。
- 整理 `Inc/m4_ipc.h`、`Src/m4_ipc.c`、`Inc/m4_isr.h`、`Src/m4_isr.c` 的实验遗留代码；保留部分接口与外部声明，正式模式不再默认创建这些演示任务或启动对应实验流程。`Inc/ring_buffer.h` / `Src/ring_buffer.c`、`debug_log` 模块继续保留。
- 将 `Src/main.c` 的 `m4_isr.h` 引入调整至 USER CODE 区；实际 Timer 源文件在忽略行尾空白后无语义 Diff。`.ioc` 仍有原 TIM7 资源配置，后续 M5 资源规划时复核，不据此认为 TIM7 已完全释放。
- CubeMX 再生成曾造成 Git 57 个修改文件；通过审核 `git diff --ignore-space-at-eol`、恢复已确认的第三方与 EOL-only 文件后，用户提供的状态为 11 个修改文件，其中 7 个存在非行尾空白差异。`git -c core.safecrlf=false diff --check` 无空白错误输出；后续建议恢复其余 4 个 EOL-only 文件，但未收到操作后的最终 `git status`。
- Debug 增量构建命令 `cmake --build --preset Debug` 成功链接并生成 ELF/HEX/BIN；固件占用 RAM 21128 B（16.12%）、FLASH 27916 B（2.66%），CCMRAM 0 B。
- 在真实开发板上完成清理后 30 秒 USART2 监控：仅 9 行，原 M4 实验日志为 0；用户确认实际应用任务数量为 2。
- 用户确认本次固件 NRST ×3、完全断电重新上电 Power-cycle ×3 均正常，提供了重启后的 UART / Heap / Stack 日志片段。

## 验收情况

- Debug 增量构建：**PASS**；本轮未提供 `--clean-first` 的独立构建记录。
- 30 秒 UART：清理前 882 行 → 清理后 9 行，减少 873 行 / 约 98.98%；清理前多模块实验输出已不再出现。
- FreeRTOS Free Heap：3432 B → 13504 B，增加 10072 B；清理后 Current/Minimum Ever Free Heap 均为 13504 B（提供的观测窗口）。
- HeartBeat Task Stack Free：384 B；UART Task Stack Free：首次可见 872 B、后续最低观测 520 B（高水位统计，不能仅根据首次数值判断余量）。
- 应用任务数：2。
- 重启后的 UART 日志有个别 `€` 前缀，后续日志保持正确；尚不能仅凭此判断为固件缺陷，若正常运行中复现需检查串口接收与解码。

## 未完成

- 把本次文档包导入本地 `docs/` 并核查 Diff；M5-P0 功能修改和文档尚未得到新的 Git Commit / Push 确认。
- 核对最终工作区是否只剩预期源码及 `.ioc` 修改；尚缺提交后的 `git status`、远程同步证据。
- M5 资源规划、ADC/Timer/DMA 驱动与实时采集链尚未开始；专项 `M4_ENABLE_SPSC_TEST=ON` 重新回归、最新固件独立 Clean Build 未在本轮提供证据。

## 今日 Bug

- CubeMX 再次引起 CRLF/LF 工作区噪音；已按实际内容差异识别并定向恢复无业务差异的文件，未观察到新的固件运行故障。
- 重启输出偶见 `€` 前缀，证据不足以确认为固件 Bug；保留为后续如复现则排查的现象。

## 今日学习

- 通过 Git Tag / 历史保存教学实验，正式运行固件只创建必要任务；关闭日志不能替代回收任务 Stack、TCB、同步对象和调度开销。
- FreeRTOS Heap 可用空间与 ELF 静态 RAM 占用不是同一指标；栈历史最小余量会随执行路径变化。
- 审核 `.ioc`、用户代码、CMake 与实际固件行为必须交叉验证，不能仅根据串口安静判断任务确实被删除。

## 尚未理解

- M5 ADC 采样时钟、Timer TRGO、DMA Stream/Channel 的资源配对，以及 Half/Full 回调的数据所有权、消费速度和丢样/覆盖检测策略。
- SPSC 的 Task ↔ Task 测试结论不能直接外推到 DMA 写入同一 Buffer 的场景。

## Git

- 当前开发分支：`feature/m5-adc-timer-dma-baseline`；本次修改前 HEAD / main / origin main 均为 `e793039`。
- 清理前基线标签：`m5-pre-cleanup-20261028`（此前工程归档中已见；本次未单独验证远程状态）。
- 本轮代码和文档仍待用户在本地审查、暂存、提交与推送；**不得填写尚不存在的新 Commit Hash**。

## 2026-10-28 M5-P1～P4 后续补充（覆盖上方当时的“尚未开始”状态）

- M5-P0 后续已完成两次独立提交并推送：`942fcc3`（`refactor: remove legacy M4 demo tasks and simplify runtime logging`）、`e7f525c`（`docs: record M5-P0 runtime cleanup verification`）；用户确认 `feature/m5-adc-timer-dma-baseline` 与对应 `origin` 同步、工作区 clean。`main` 保持 `e793039`。
- 使用 10 kΩ 电位器：两固定端连接 3.3 V / GND，滑动端接 PA1 / ADC1_IN1；避免 PA0/WKUP 引脚冲突。ADC1 12-bit、单通道 Regular，TIM2 TRGO Update 上升沿外部触发，ADC 连续转换关闭、DMA 连续请求开启。
- TIM2 APB1 Timer Clock 84 MHz、PSC=8399、ARR=199，设计采样频率 50 Hz；不经 TIM2 更新 ISR 软件启动 ADC。ADC 时钟按 APB2 84 MHz /4 = 21 MHz 配置，Sampling Time 从 3 cycles 调整至 84 cycles。
- DMA2 Stream0、Peripheral-to-Memory、Circular、半字宽、内存递增、FIFO 关闭，NVIC Preemption 5 / Sub 0；Buffer 为 `uint16_t[20]`、Half/Full 各 10 个采样。新增 FreeRTOS `AcquisitionTask`，使用 Thread Flags 通知，在 Task 中计算 10 点平均值并按 3300 mV 参考值换算电压；ISR 回调只计数/发通知/记时间，未在 ISR 同步打印 UART。
- ADC 旋转电位器得到 `2, 612, 1065, 1451, 2250, 2974, 3287, 3285, 3286 mV`；最大端连续三点变化范围 2 mV。未提供万用表同步数据，不能判定绝对测量精度。
- DMA Half/Full 回调在相邻约 10 秒监控窗口分别 +25 次，`Error=0`；回调与采集任务处理计数对比正常（个别采样瞬间差 1）。
- 开展 0 / 50 / 250 Tick（按约 1 ms RTOS Tick 口径为 0 / 50 / 250 ms）人为 `osDelay()` 压力测试；250 Tick 放在 `osThreadFlagsWait()` 之后、读取 Buffer 之前。0/50 组 `Late=0`；250 组 `Late=27→57→87`，成功检出处理超时风险，未能证明逐样本真实覆盖或 Flag 丢失数量。
- 结束压力测试后已恢复无人工延时的正式采集路径；Debug 构建成功（增量链接），RAM `21448 B`（16.36%）、FLASH `35680 B`（3.40%）、CCMRAM `0`；正常回归最后 `Half=73/73 Full=72/72 Late=0 Error=0`，Heap/MinHeap `12368 B`、HeartFree `384 B`、UARTFree 最低可见 `512 B`。

## 明天下一步

继续 M5-P5 长时间稳定性和复位回归；按真实测试记录归档并检查 ADC/TIM2/DMA/AcquisitionTask 相关工作区 Diff，生成新的功能与文档 checkpoint。

# 2026-10-29

## 今日计划

- 完成 M5 ADC + TIM2 TRGO + DMA Circular + FreeRTOS 的正常运行回归与稳定性验收。
- 结束 DMA 延时压力实验，确认正式运行态不残留 `osDelay(250U)`。
- 将实测结果按既有测试/状态文档格式归档，准备独立的功能与文档提交。

## 实际完成

- 在不额外引入 Queue 或软件双缓冲区的前提下，完成 DMA 回调数与任务处理数对照、超时计数（Late）及人工调度延时对比实验。
- A 组 0 Tick：最后 `Half=73/73 Full=72/72 Late=0 Error=0`；B 组 50 Tick：最后 `Half=72/73 Full=72/72 Late=0 Error=0`；C 组 250 Tick：最后 `Half=72/73 Full=71/72 Late=87 Error=0`，Late 观测值依次为 27、57、87。
- 人为延时已移除，恢复常规读取流程；再次增量构建 Debug 成功，LED/UART 正常，正常运行最后采样 `Half=73/73 Full=72/72 Late=0 Error=0`，平均电压约 3284～3288 mV。
- 用户确认本次正式采集固件长时间运行测试通过，NRST ×3 和完全断电重新上电 Power-cycle ×3 均通过；实际长时间测试的起止时刻与分钟数未提供，文档不虚构具体持续时间。

## 验收情况

- 最终已提供构建：`cmake --build --preset Debug`，输出 `[2/2] Linking`；RAM `21448 B (16.36%)`、FLASH `35680 B (3.40%)`、CCMRAM `0 B`。
- 当前 FreeRTOS Heap/MinHeap 观测均为 `12368 B`，HeartFree `384 B`、UARTFree 最低可见 `512 B`；观测期间无持续下降。
- 低负载与 50 Tick 延时：Late 均为 0；250 Tick 延时：Late 持续增长且 ADC Error 仍为 0，验证了软件处理超时检测的有效性，而非证明 ADC 硬件故障。
- 恢复正常运行后 Half/Full 回调数与已处理数一致，Late/Error 均为 0；长时间运行和 6 次复位由用户确认 PASS。
- M5-P1～P5 的**既定基础功能与初步可靠性实验**已完成；样本级无丢失、最坏调度延迟与工业精度标定不属于本次已验证结论。

## 未完成

- 尚未收到本次 ADC + TIM2 + DMA 功能代码的新 Git Commit/Push、最新 `git status`、暂存区 Diff 或 `--clean-first` 构建证据；不要将本次 docs ZIP 视为已提交。
- 长时间运行具体时长、精密频率测量和外部万用表/校准源电压精度对照未提供。
- DMA Circular 覆盖的逐样本证明、Thread Flags 合并时的严格无丢块保证和最坏响应时间专项未完成；未来按真实需求再扩展。

## 今日 Bug

- 无已确认的新固件 Bug；250 Tick 人为延时引起 `Late` 增长是预期压力注入结果，不作为正常固件缺陷。

## 今日学习

- DMA Circular 缓冲区即使不越界，也可能在任务晚读时被覆盖；ADC `Error=0` 不等于无数据丢失。
- Half/Full 回调计数与任务处理计数可检查处理积压，但差 1 次可能只是 UART 读取状态的时间边界；计数相等不能严格证明原样本未覆盖。
- Tick 时间戳的简化超时统计在通知合并/时间戳更新情况下可能漏报；阶段性验收需明确边界。

## 尚未理解

- 如何以尽量小的代价做 DMA 半区所有权或逐块序号追踪，并量化最坏调度延迟；见 `KG-013` / `KG-014`。
- ADC 绝对精度、VREF 误差、采样源阻抗和完整工业级信号调理留待后续实际需求。

## Git

- 已知上一次确认干净且同步的 M5 开发分支 HEAD 为 `e7f525c`；上一功能重构提交为 `942fcc3`，主分支仍为 `e793039`。
- 本次 ADC/Timer/DMA 新开发已有板级运行证据，但**本轮功能代码与新文档尚未提交/推送**，不虚构新 Commit Hash。

## 明天下一步

先保存稳定运行的代码快照，执行 `git status --short`、`git diff --ignore-space-at-eol --stat`、`git diff --check`；复核 `.ioc` 与新增生成代码、确认无压力测试专用延时；按功能提交和测试文档提交分批暂存、审核、Commit/Push，再根据路线图决定是否进入下一里程碑。
