# 测试记录

> 唯一回答：怎么证明它工作？
> 更新时机：进行正式、可复现的验证时。

## TEST-20260906-01：开发工具链可用性

- 目标：确认 M0 计划采用的命令行工具可执行。
- 环境：Windows PowerShell，STM32CubeCLT 1.22.0。
- 方法：查询工具路径并执行版本命令。
- 实际结果：
  - Git 2.54.0.windows.1
  - STM32CubeMX 6.11.1
  - CMake 4.3.1
  - Ninja 1.13.2
  - GNU Arm GCC 14.3.1
  - GNU assembler 2.44.0.20250616
  - GNU ld 2.44.0.20250616
  - 找到 STM32CubeProgrammer CLI 和 ST-LINK GDB Server
- 结论：通过。
- 边界：本测试只证明工具可启动，不证明任何固件能够编译或运行。

## TEST-20260906-02：Git 仓库初始化

- 目标：确认仓库建立在指定目录且初始分支正确。
- 方法：执行 `git rev-parse --show-toplevel`、`git branch --show-current` 和 `git status --short --branch`。
- 预期：仓库根目录为 `STM32F407_Industrial_Platform`，分支为 `main`，尚无提交。
- 实际结果：与预期一致。
- 结论：通过。

## TEST-20260906-03：Git 忽略规则

- 目标：确认构建产物被忽略，工程输入不被误忽略。
- 方法：使用 `git check-ignore -v --no-index` 检查示例路径。
- 实际结果：
  - `build/probe.txt` 命中 `/build/`。
  - `cmake-build-debug/probe.txt` 命中 `/cmake-build-*/`。
  - `firmware.elf` 命中 `*.elf`。
- `Core/Src/main.c`、`Project.ioc` 和 `CMakeLists.txt` 未命中忽略规则。
- 结论：通过。

## TEST-20260906-04：GitHub 远程同步

- 目标：证明第一次稳定提交已存在于 GitHub 远程 `main`。
- 方法：比较本地 `HEAD`、本地远程跟踪引用 `origin/main`，并使用 `git ls-remote --heads origin main` 直接查询 GitHub。
- 预期：三者指向同一完整提交哈希。
- 实际结果：三者均指向 `8bc8d98f36bf73577daaf4e3d70656f15b18b5db`。
- 结论：通过。
- 边界：Git 引用查询不能证明仓库可见性；Private 属性由 GitHub 仓库设置确认。

## TEST-20260908-01：M1 硬件配置输入核对

- 目标：在生成 CubeMX 工程前确认 MCU、时钟、LED、UART、SWD 和供电配置。
- 方法：交叉检查开发板原理图、引脚分配图、厂商资料、STM32F407 数据手册及用户确认的实物连接。
- 实际结果：
  - 目标 MCU 为 STM32F407VGT6，LQFP-100。
  - HSE 为 8 MHz，LSE 为 32.768 kHz。
  - PB2/BOOT1 经 LED 和限流电阻接地，高电平点亮，并有 10 kΩ 下拉。
  - PD5/PD6 分别支持 USART2_TX/USART2_RX。
  - ST-LINK V2 使用 SWD，并连接 NRST。
  - 板端使用降压后的稳定 5 V 输入，不直接接入 12 V。
- 结论：通过，可作为 CubeMX 配置输入。
- 边界：本次只完成文档与连接信息核对，尚未证明时钟、GPIO、UART、下载或复位在实物上工作。

## TEST-20260915-01：CubeMX 最小裸机工程配置核对

- 目标：确认实际生成的 CubeMX 工程仍然符合已经确认的 M1 硬件基线。
- 方法：检查 .ioc、生成的 GPIO、USART 和时钟相关配置。
- 实际结果：
  - 目标器件为 STM32F407VGT6，LQFP-100。
  - HSE 配置值为 8 MHz。
  - PLL 使用 HSE 作为输入，SYSCLK/HCLK 为 168 MHz。
  - APB1 为 42 MHz，APB2 为 84 MHz。
  - PB2 配置为 GPIO 输出，标签为 USER_LED。
  - PD5/PD6 分别配置为 USART2_TX/USART2_RX。
  - USART2 配置为 115200、8 数据位、1 停止位、无校验、收发模式。
- 结论：通过，当前生成工程与 M1 已确认硬件输入一致。
- 边界：本测试只验证生成配置和源码内容，不证明这些配置已经在实物硬件上正确工作。

## TEST-20260915-02：M1 CMake + Ninja + GCC 构建基线
- 目标：证明 CubeMX 生成的 STM32F407VGT6 裸机工程可以通过项目规定的命令行工具链完整构建。
- 环境：Windows PowerShell，STM32CubeCLT 1.22.0，GNU Arm Toolchain，CMake，Ninja。
- 方法：使用 CMake Debug preset 配置工程，并通过 Ninja 详细构建输出检查 C 源码、启动汇编、链接和固件产物生成过程。
- 实际结果：
  - CMake 能够正确使用项目 CMake 配置。
  - Ninja 能够启动并执行完整构建任务。
  - arm-none-eabi-gcc 能够完成 STM32F407xx HAL 工程源码编译。
  - 工程能够成功完成链接。
  - 成功生成 STM32F407_Industrial_Platform.elf。
  - 成功生成对应 .hex 和 .bin 固件产物。
  - build/ 被 Git 忽略，不进入源码版本历史。
- 结论：通过。当前工程已经达到“可构建”状态。
- 边界：本测试只能证明源代码和构建系统能够产生固件映像，不证明固件已成功下载，也不证明系统时钟、LED、USART2 或其他外设已经在目标板上正常工作。

