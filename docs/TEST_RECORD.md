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

## TEST-20260917-01：PB2 LED 心跳板级验证

- 目标：证明当前 STM32F407VGT6 裸机固件能够成功下载、启动，并在真实硬件上驱动 PB2 USER_LED 周期翻转。
- 环境：STM32F407VGT6 开发板，ST-LINK V2，当前 `feature/m1-bare-metal-baseline` 分支，CMake + Ninja + GNU Arm GCC。
- 方法：
  - 在 CubeMX 保留区域中加入 PB2 LED 最小心跳逻辑。
  - 使用 `cmake --build --preset Debug` 构建固件。
  - 使用 ST-LINK 将固件下载到目标板。
  - 观察 PB2 USER_LED 是否按照程序设定每 500 ms 翻转一次。
  - 连续执行 3 次 NRST，检查程序是否能够恢复正常运行。
- 实际结果：
  - 固件构建成功。
  - 固件成功下载到 STM32F407VGT6。
  - PB2 USER_LED 能够按照设定的 500 ms 延时周期稳定翻转。
  - 连续 3 次 NRST 后，LED 心跳均能够重新正常运行。
- 结论：通过。当前固件已经证明能够在目标 STM32F407VGT6 上启动运行，PB2 GPIO 输出链路和 HAL 基础时基可以支持当前最小心跳功能。
- 边界：
  - 500 ms 为软件设定延时值，未使用示波器或逻辑分析仪测量实际翻转时间，因此不能证明其精确时序。
  - 本测试只证明 PB2 相关 GPIO 输出路径有效，不代表其他 GPIO 配置全部通过验证。

## TEST-20260917-02：USART2 TX 板级验证

- 目标：证明当前裸机固件能够通过 USART2 从 STM32F407VGT6 向 PC 稳定发送调试信息。
- 环境：STM32F407VGT6 开发板，3.3 V USB-TTL，PC 串口终端，USART2 配置为 115200、8N1、无硬件流控。
- 方法：
  - USART2_TX 使用 PD5。
  - 开发板 PD5/TX 连接 USB-TTL RX，双方 GND 共地。
  - 在 `MX_USART2_UART_Init()` 完成后，通过 `HAL_UART_Transmit()` 阻塞发送一次 `USART2 TX OK\r\n`。
  - PC 串口终端配置为 115200 8N1、无流控。
  - 连续执行 3 次 NRST，检查每次复位后是否均能收到启动信息，同时观察 PB2 heartbeat 是否继续正常运行。
- 实际结果：
  - PC 串口终端成功收到 `USART2 TX OK`。
  - 连续 3 次 NRST 后，每次均能够重新收到启动字符串。
  - USART2 TX 工作期间 PB2 LED heartbeat 保持正常。
- 结论：通过。USART2 TX、PD5 输出、USB-TTL 和 PC 串口终端组成的发送链路已经通过真实硬件验证。
- 边界：
  - 本次只验证 USART2 TX。
  - USART2 RX 尚未进行实际接收测试，因此不能宣称 USART2 双向通信已经验证。

## TEST-20260917-03：复位与断电重启稳定性验证

- 目标：验证当前裸机基线在 NRST 和完全断电重新上电后能够重复恢复到正常运行状态。
- 方法：
  - 在 LED heartbeat 和 USART2 TX 均已验证的固件上连续执行 3 次 NRST。
  - 完全断开开发板供电并重新上电，共执行 3 次。
  - 每次启动后同时检查 PB2 LED heartbeat 和 USART2 TX 启动信息。
- 预期：
  - 每次复位或重新上电后程序均能够重新启动。
  - PB2 heartbeat 恢复正常。
  - USART2 TX 能重新发送启动信息。
- 实际结果：
  - NRST 连续 3 次均正常恢复运行。
  - 完全断电重新上电连续 3 次均正常恢复运行。
  - 每次启动后 PB2 heartbeat 和 USART2 TX 均正常。
- 结论：通过。当前 M1 裸机固件已经具备基本的可重复启动和可观测运行能力。
- 边界：本测试只验证当前短周期人工复位和断电重启场景，不代表已经完成长时间运行、异常恢复或可靠性测试。

## TEST-20260920-01：FreeRTOS 单任务调度基线验证

- 目标：验证 FreeRTOS 能够在 STM32F407VGT6 上正常启动 scheduler 并运行最小 HeartbeatTask。
- 配置：
  - CMSIS-RTOS2
  - FreeRTOS Tick：1000 Hz
  - HAL Timebase：TIM6
  - HeartbeatTask：osPriorityNormal，Stack 128 Words
- 方法：
  - HeartbeatTask 周期翻转 PB2 USER_LED。
  - 使用 `osDelay(500U)` 阻塞任务。
  - USART2 在 scheduler 启动前发送一次 `USART2 TX OK`。
- 实际结果：
  - CMake/Ninja 构建成功。
  - ELF/HEX/BIN 均生成。
  - 固件成功烧录。
  - 上电/复位后 USART2 启动信息正常。
  - HeartbeatTask 能持续运行，PB2 LED 按设定约 500 ms 翻转。
  - NRST ×3 通过。
  - 完全断电重新上电 ×3 通过。
- 结论：通过。FreeRTOS scheduler、单用户任务运行及阻塞延时路径已在真实硬件上验证。
- 边界：
  - 尚未验证两个用户任务之间的调度。
  - 尚未验证任务优先级、抢占、同步与通信。
  - 500 ms 为软件设定值，未进行精密时序测量。

## TEST-20260920-02：FreeRTOS 双用户任务调度基线验证

- 目标：验证两个同优先级 FreeRTOS 用户任务能够在 STM32F407VGT6 上稳定共存运行。
- 配置：
  - HeartbeatTask：osPriorityNormal，使用 `osDelay(500U)`。
  - UARTTask：osPriorityNormal，使用 `osDelay(1000U)`。
  - FreeRTOS Tick：1000 Hz。
  - HAL Timebase：TIM6。
- 方法：
  - HeartbeatTask 周期翻转 PB2 USER_LED。
  - UARTTask 周期通过 USART2 发送测试字符串。
  - 同时观察 LED 与串口输出。
  - 连续执行 3 次 NRST。
  - 连续执行 3 次完全断电重新上电。
- 实际结果：
  - LED heartbeat 持续正常。
  - UART 周期输出持续正常。
  - 两个用户任务能够同时长期运行。
  - NRST ×3 均正常恢复。
  - Power-cycle ×3 均正常恢复。
- 结论：通过。两个同优先级用户任务已在真实硬件上完成稳定运行验证，FreeRTOS 基本多任务调度能力成立。
- 边界：
  - 尚未单独验证不同优先级之间的抢占行为。
  - 尚未验证时间片轮转的精确切换时刻。
  - 尚未验证任务间同步、通信和共享资源竞争。

### TEST-20260922-01 — FreeRTOS 不同优先级抢占验证

- 目标：验证 FreeRTOS 抢占式调度：当低优先级任务持续运行且不主动阻塞或让出 CPU 时，高优先级任务从 Blocked 状态恢复为 Ready 后能够抢占低优先级任务运行。
- 配置：
  - HeartbeatTask：osPriorityNormal，无 `osDelay()`、`osThreadYield()`和其他主动阻塞操作。
  - UARTTask：osPriorityAboveNorma，每轮使用 `osDelay(1000U)`。
  - `low_counter`：volatile uint32_t。
- 方法：
  - HeartbeatTask：持续执行 `low_counter++`。
  - UARTTask：周期打印 `low_counter`和`USART2 TX OK`，同时翻转 USER_LED。
  - `low_counter`：HeartbeatTask 写入后 UARTTask 读取。
  - 连续执行 3 次 NRST。
  - 连续执行 3 次完全断电重新上电。
- 实际结果：
  - 连续运行时 UARTTask 能够稳定周期执行，LED 正常翻转，UART 正常输出。
  - 部分串口数据：
      ```text
      USART2 TX OK  low_counter = 234449906
      USART2 TX OK  low_counter = 251196147
      USART2 TX OK  low_counter = 267942352
      USART2 TX OK  low_counter = 284688672
      USART2 TX OK  low_counter = 301434957
      USART2 TX OK  low_counter = 318181206
      USART2 TX OK  low_counter = 334927430
      USART2 TX OK  low_counter = 351673758
      ```
      相邻采样之间 `low_counter` 大约增加 1674 万，表明 UARTTask 处于 Blocked 状态时 HeartbeatTask 持续获得 CPU 运行。
  - 两个用户任务能够同时长期运行。
  - NRST ×3 均正常恢复。
  - 复位后的典型输出：
      ```text
      USART2 TX OK  low_counter = 0
      USART2 TX OK  low_counter = 16741181
      USART2 TX OK  low_counter = 33488882
      USART2 TX OK  low_counter = 50236544
      ```
  - Power-cycle ×3 均正常恢复。
- 结论：通过，不同优先级的两个任务持续运行正常，高优先级成功抢占CPU，FreeRTOS 抢占式优先级调度行为成功实现。
- 边界：
  - 未验证同优先级任务时间片轮转；
  - 未实现UART 多任务并发访问；
  - 未接入 Mutex / Semaphore / Queue；
  - 未确认 Task Stack 是否具有足够长期安全余量；
  - 未确认 FreeRTOS Heap 长期使用情况。

## TEST-20260922-02：FreeRTOS Task Stack 运行时余量验证

- 目标：
  - 验证 FreeRTOS 用户任务的运行时 Stack 历史最小剩余空间能够被观测。
  - 评估 HeartbeatTask 和 UARTTask 当前 Stack 配置是否具有合理余量。
  - 对 UARTTask 初始 Stack 余量不足问题进行调整并重新验证。

- 配置：
  - HeartbeatTask：
    - Priority：`osPriorityNormal`
    - Stack：128 Words（512 B）
  - UARTTask：
    - Priority：`osPriorityAboveNormal`
    - 初始 Stack：128 Words（512 B）
  - HeartbeatTask 持续执行 `low_counter++`，不主动阻塞或让出 CPU。
  - UARTTask 周期执行格式化输出、USART2 发送、LED 翻转及 `osDelay(1000U)`。
  - 使用 `osThreadGetStackSpace()` 获取任务历史最小剩余 Stack 空间。

- 方法：
  - 保持已验证的不同优先级抢占任务结构。
  - UARTTask 启动后先运行多个周期，使实际函数调用路径得到执行。
  - 周期读取 HeartbeatTask 和 UARTTask 的历史最小剩余 Stack。
  - 首先测量 UARTTask 为 512 B Stack 时的运行余量。
  - 根据首次测试结果，将 UARTTask Stack 从 128 Words（512 B）增加至 256 Words（1024 B）。
  - 重新构建、烧录并连续运行，验证调整后的 Stack 余量。

- 初始测试结果：
  - HeartbeatTask：
    - 配置 Stack：512 B
    - 历史最小剩余：432 B
    - 历史峰值使用约：80 B
    - 峰值使用率约：15.6%
  - UARTTask：
    - 配置 Stack：512 B
    - 历史最小剩余：64 B
    - 历史峰值使用约：448 B
    - 峰值使用率约：87.5%
  - UARTTask 仅剩 64 B 历史最小余量，不适合作为后续功能扩展的稳定 Stack 配置。

- 调整：
  - 将 UARTTask Stack 从 128 Words（512 B）增加至 256 Words（1024 B）。
  - HeartbeatTask Stack 保持 128 Words（512 B）不变。

- 调整后实际结果：
  - HeartbeatTask 历史最小剩余 Stack 稳定为 432 B。
  - UARTTask 在仅启用 Stack 观测时历史最小剩余 Stack 稳定为 576 B。
  - 加入 Heap 观测路径后，UARTTask 历史最小剩余 Stack 稳定为 560 B。
  - UARTTask 当前完整测试路径历史峰值 Stack 使用量约为：
    - 1024 B - 560 B = 464 B
    - 峰值使用率约 45.3%
  - 新增 Heap 观测调用路径后，UARTTask 峰值 Stack 使用量相比仅进行 Stack 观测时增加约 16 B。
  - HeartbeatTask 当前完整测试路径历史峰值 Stack 使用量仍约为 80 B。
  - 连续多轮运行期间 Stack 数据稳定。

- 构建结果：
  - RAM：21024 B / 128 KB，16.04%
  - FLASH：24712 B / 1 MB，2.36%

- 结论：
  - PASS。
  - `osThreadGetStackSpace()` 能够稳定观测任务历史最小剩余 Stack。
  - HeartbeatTask 当前 512 B Stack 具有充足余量。
  - UARTTask 原 512 B Stack 余量偏小。
  - UARTTask 调整至 1024 B 后，在当前完整测试路径下历史最小剩余约 560 B，具备明显更合理的开发余量。
  - Task Stack 使用量会随实际调用路径变化，后续增加功能后仍需重新测量。

- 边界：
  - 当前结果仅覆盖 M2 当前实际执行路径。
  - 尚未包含 Queue、Mutex、Task Notification、ADC/DMA、协议处理及后续业务任务带来的 Stack 消耗。
  - 当前测试不能证明未来完整系统中 1024 B UARTTask Stack 永久足够。
  - 后续重要功能加入后应重新检查 Stack High Water Mark。

## TEST-20260922-03：FreeRTOS Heap 运行时余量验证

- 目标：
  - 验证当前 M2 系统中 FreeRTOS Heap 的运行时占用情况。
  - 获取当前剩余 Heap 和系统启动以来历史最小剩余 Heap。
  - 验证系统连续运行及 NRST 重启后 Heap 状态是否稳定、可重复。

- 配置：
  - `configTOTAL_HEAP_SIZE`：15360 B。
  - HeartbeatTask：
    - Priority：`osPriorityNormal`
    - Stack：512 B
  - UARTTask：
    - Priority：`osPriorityAboveNormal`
    - Stack：1024 B
  - 使用 `xPortGetFreeHeapSize()` 获取当前剩余 FreeRTOS Heap。
  - 使用 `xPortGetMinimumEverFreeHeapSize()` 获取历史最小剩余 FreeRTOS Heap。
  - 同时继续使用 `osThreadGetStackSpace()` 观测两个用户任务的 Stack 余量。

- 方法：
  - 保持当前抢占调度和 Stack 观测任务结构不变。
  - UARTTask 周期读取当前 FreeRTOS Heap 剩余量和历史最低剩余量。
  - 通过 USART2 周期输出 Heap 和 Task Stack 数据。
  - 连续运行多个周期观察数值稳定性。
  - 执行 NRST 三次，验证每次重新启动后的资源状态是否可重复。

- 实际结果：
  - FreeRTOS Heap 总大小：15360 B。
  - Current Free Heap：13592 B。
  - Minimum Ever Free Heap：13592 B。
  - 当前 FreeRTOS Heap 已使用约：
    - 15360 B - 13592 B = 1768 B
    - 使用率约 11.51%
  - 当前 FreeRTOS Heap 剩余约：
    - 13592 B
    - 剩余率约 88.49%
  - 连续运行期间 `Heap` 与 `MinHeap` 始终稳定为 13592 B。
  - HeartbeatTask 历史最小剩余 Stack 稳定为 432 B。
  - UARTTask 历史最小剩余 Stack 稳定为 560 B。
  - UARTTask 当前完整测试路径历史峰值 Stack 使用量约为：
    - 1024 B - 560 B = 464 B
    - 峰值使用率约 45.3%
  - 执行 NRST 三次后：
    - Current Free Heap 均重新稳定为 13592 B。
    - Minimum Ever Free Heap 均重新稳定为 13592 B。
    - HeartbeatTask 历史最小剩余 Stack 均为 432 B。
    - UARTTask 历史最小剩余 Stack 均为 560 B。
  - 三次重启后的资源状态具有一致性和可重复性。

- 构建结果：
  - RAM：21024 B / 128 KB，16.04%
  - FLASH：24712 B / 1 MB，2.36%

- 结论：
  - PASS。
  - `xPortGetFreeHeapSize()` 与 `xPortGetMinimumEverFreeHeapSize()` 能够稳定观测当前 FreeRTOS Heap 状态。
  - 当前 M2 基线中 FreeRTOS Heap 已使用约 1768 B，约占总 Heap 的 11.51%。
  - 当前剩余 Heap 约 13592 B，占总 Heap 的约 88.49%。
  - 当前测试期间 Current Free Heap 与 Minimum Ever Free Heap 一致，未观察到系统启动完成后进一步降低历史最低 Heap 的动态分配行为。
  - NRST 三次后的 Heap 与 Stack 数据均能够恢复到相同稳定值。
  - 当前 M2 系统具有较充足的 FreeRTOS Heap 余量。

- 边界：
  - 当前 Heap 使用情况仅代表 M2 当前任务和 RTOS 对象配置。
  - 尚未加入 Queue、Mutex、Semaphore、Task Notification、ADC/DMA 及后续业务任务。
  - 后续创建新的 RTOS 对象后，应重新检查 Current Free Heap 和 Minimum Ever Free Heap。
  - 当前 88.49% 的剩余 Heap 不代表完整系统阶段仍会保持相同余量。

## TEST-20260924-01：FreeRTOS 同优先级时间片调度验证

- 目标：
  - 验证两个相同优先级、持续处于 Ready 状态且不主动阻塞或 yield 的任务能够持续获得 CPU 执行机会。
  - 验证当前工程实际启用了 FreeRTOS 抢占式时间片调度。
  - 同时确认更高优先级任务从 Blocked 恢复为 Ready 后仍能够正常抢占同优先级 Busy Task。

- 配置：
  - `configUSE_PREEMPTION = 1`，由 `FreeRTOSConfig.h` 显式定义。
  - `configUSE_TIME_SLICING = 1`，当前 `FreeRTOSConfig.h` 未显式定义，由 FreeRTOS 默认配置提供。
  - BusyTaskA：
    - Priority：`osPriorityNormal`
    - Stack：512 B
    - 持续执行 `busy_counter_a++`
    - 不调用 `osDelay()` 或 `osThreadYield()`
  - BusyTaskB：
    - Priority：`osPriorityNormal`
    - Stack：512 B
    - 持续执行 `busy_counter_b++`
    - 不调用 `osDelay()` 或 `osThreadYield()`
  - UARTTask：
    - Priority：`osPriorityAboveNormal`
    - Stack：1024 B
    - 周期读取并输出两个 Busy Task 的计数器
    - 每轮调用 `osDelay(1000U)`

- 方法：
  - 使用 `git grep` 核对 `configUSE_PREEMPTION` 和 `configUSE_TIME_SLICING` 的最终有效配置。
  - 创建两个相同优先级的 Busy Task，使二者始终保持 Ready/Running 竞争状态。
  - UARTTask 以更高优先级每约 1000 ms 唤醒一次，输出两个计数器后重新进入 Blocked。
  - 观察两个计数器是否均持续增长。
  - 连续执行 3 次 NRST，检查调度行为能否稳定重复。

- 实际结果：
  - Scheduler 启动后的首次串口输出为：
    ```text
    busy_counter_a = 0 busy_counter_b = 0
    ```
    与 UARTTask 优先级最高、首先获得 CPU 的预期一致。
  - 后续部分输出：
    ```text
    busy_counter_a = 8358950 busy_counter_b = 8366245
    busy_counter_a = 16717052 busy_counter_b = 16732490
    busy_counter_a = 25072256 busy_counter_b = 25098735
    busy_counter_a = 33427487 busy_counter_b = 33464980
    busy_counter_a = 41782744 busy_counter_b = 41831225
    busy_counter_a = 50138018 busy_counter_b = 50197470
    ```
  - BusyTaskA 和 BusyTaskB 的计数器均持续增长。
  - 两个计数器数值接近但不要求完全相等。
  - UARTTask 能够持续周期唤醒并输出数据，说明更高优先级 Ready Task 仍能够正常抢占 BusyTaskA/BusyTaskB。
  - 连续 3 次 NRST 后均重新从 `0 / 0` 开始，并再次出现两个计数器持续增长的相同行为。
  - 未观察到 HardFault、任务卡死或某一个 Busy Task 长期得不到执行机会。

- 构建结果：
  - RAM：20992 B / 128 KB，16.02%
  - FLASH：24364 B / 1 MB，2.32%

- 结论：
  - PASS。
  - 当前工程最终有效配置满足 `configUSE_PREEMPTION = 1` 和 `configUSE_TIME_SLICING = 1`。
  - 两个相同优先级、持续 Ready 的任务均能够持续获得 CPU 执行机会，实际行为与 FreeRTOS 同优先级时间片调度机制一致。
  - 更高优先级 UARTTask 从 Blocked 恢复为 Ready 后能够正常抢占两个 Normal 优先级 Busy Task。
  - 本实验同时进一步验证了同优先级时间片调度和不同优先级抢占行为。

- 边界：
  - 本实验只证明两个同优先级 Ready Task 均持续获得执行机会，不证明两个任务精确各获得 50% CPU 时间。
  - 未使用 GPIO、逻辑分析仪或 Trace 工具测量具体时间片长度和上下文切换时刻。
  - BusyTaskA/BusyTaskB 为调度验证用临时代码，不作为最终 M2 工程基线保留。

## TEST-20260924-02：M2 FreeRTOS 最终基线稳定性验证

- 目标：
  - 将 M2 调度实验代码恢复为正常周期任务结构。
  - 验证最终 FreeRTOS baseline 的 HeartBeatTask、UARTTask、Task Stack 和 FreeRTOS Heap 监测能够稳定工作。
  - 验证最终 baseline 在 NRST 和完全断电重新上电后具有可重复运行能力。

- 配置：
  - HeartBeatTask：
    - Priority：`osPriorityNormal`
    - Stack：512 B
    - 周期翻转 PB2 USER_LED
    - 使用 `osDelay(500U)`
  - UARTTask：
    - Priority：`osPriorityNormal`
    - Stack：1024 B
    - 周期输出 `USART2 TX OK`
    - 使用 `osDelay(1000U)`
  - UARTTask 在前几个运行周期后开始周期读取：
    - `osThreadGetStackSpace()`
    - `xPortGetFreeHeapSize()`
    - `xPortGetMinimumEverFreeHeapSize()`

- 方法：
  - 删除 BusyTaskA、BusyTaskB 和调度实验计数器。
  - 恢复 HeartBeatTask 正常 PB2 heartbeat。
  - 将 UARTTask 恢复为 `osPriorityNormal`。
  - 保留经过验证的 UARTTask 1024 B Stack 配置。
  - 保留 Task Stack 与 FreeRTOS Heap 运行时监测。
  - 重新构建、烧录并观察 LED、UART 和资源数据。
  - 连续执行 3 次 NRST。
  - 连续执行 3 次完全断电重新上电。

- 实际结果：
  - PB2 USER_LED heartbeat 持续正常。
  - USART2 能够持续周期输出 `USART2 TX OK`。
  - 稳定运行后资源数据为：
    ```text
    HeartFree = 384 B
    UARTFree = 560 B
    Heap = 13592 B
    MinHeap = 13592 B
    ```
  - HeartBeatTask：
    - 配置 Stack：512 B
    - 历史最小剩余：384 B
    - 历史峰值使用约：128 B
  - UARTTask：
    - 配置 Stack：1024 B
    - 第一次正式资源采样时可观察到约 816 B 剩余
    - 首次执行后续 `snprintf()` 格式化输出路径后，历史最小剩余更新为 560 B
    - 后续持续稳定为 560 B
    - 历史峰值使用约：464 B
  - `txt_buff` 为 `static` 局部数组，不位于 UARTTask Stack；UARTFree 从 816 B 更新为 560 B 的原因是首次执行 `snprintf()` 等调用路径后产生了更深的历史 Stack 使用，而不是字符串缓冲区从空变为有数据。
  - Current Free Heap 持续为 13592 B。
  - Minimum Ever Free Heap 持续为 13592 B。
  - 连续运行期间未观察到 Heap 持续下降。
  - NRST ×3 均正常恢复。
  - Power-cycle ×3 均正常恢复。
  - 每次完全重新启动后资源观测过程均能重复出现。
  - 未观察到 HardFault、Hang 或任务异常停止。

- 结论：
  - PASS。
  - M2 调度实验代码已成功恢复为正常周期任务 baseline。
  - HeartBeatTask 与 UARTTask 均能够稳定运行并主动进入 Blocked 状态。
  - 当前 Task Stack 配置具有充足的 M2 开发余量。
  - 当前 FreeRTOS Heap 状态稳定。
  - M2 最终代码基线已经达到可构建、可烧录、可运行、可观测和可重复启动的验收状态。
  - 最终功能 checkpoint 为 `9c7333b`：`feat: finalize FreeRTOS scheduling baseline`。

- 边界：
  - 当前 Stack 与 Heap 数据只代表 M2 当前执行路径。
  - 尚未加入 Queue、Mutex、Task Notification、ADC/DMA、USART RX、RS485、Modbus 等后续功能。
  - 当前测试不能证明未来完整系统中现有 Stack 和 Heap 配置永久足够。
  - 后续新增重要任务或 RTOS 对象后必须重新检查 Stack High Water Mark 和 FreeRTOS Heap。
