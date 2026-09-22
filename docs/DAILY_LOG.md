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