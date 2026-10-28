# 项目路线图

> 唯一回答：整个项目以后往哪走？
> 更新时机：里程碑目标或顺序发生变化时。

## 项目定位

本项目定位为：

> 基于 STM32F407VGT6 + FreeRTOS 的工业实时数据采集与多总线通信平台。

主线能力包括 MCU 工程基线、RTOS 并发、实时数据采集、UART/RS485/Modbus、CAN、Bootloader/IAP、故障诊断、系统可靠性以及上位机数据链路。项目保持 MCU / STM32 / FreeRTOS 主线，采用 Just-in-time implementation：只在进入对应里程碑后实现所需功能，不提前堆叠空模块或未经验证的硬件配置。

## 状态说明

- `当前`：正在推进或准备正式开始。
- `待开始`：尚未进入，只定义目标与验收边界。
- `完成`：已有实现和验证记录支撑，达到当时定义的验收标准。

## 里程碑总览

| 里程碑 | 目标 | 状态 |
| --- | --- | --- |
| M0 | 建立开发环境、项目管理体系和可复现工程基线 | 完成 |
| M1 | 建立可下载、可运行、可观测的 STM32 裸机 Known Good Baseline | 完成 |
| M2 | 启动并理解 FreeRTOS 最小系统与调度基础 | 完成 |
| M3 | 建立线程安全、可裁剪的调试基础设施 | 完成 |
| M4 | 掌握并实现 RTOS 通信、同步和数据缓冲基础 | 完成 |
| M5 | 建立 ADC + Timer + DMA 实时数据采集子系统 | 当前（P0 已板级验收，待 Git 归档） |
| M6 | 实现并验证 UART DMA + IDLE 不定长通信 | 待开始 |
| M7 | 实现并验证 RS485 半双工通信 | 待开始 |
| M8 | 实现基础 Modbus RTU 通信 | 待开始 |
| M9 | 建立独立于具体总线的统一数据模型 | 待开始 |
| M10 | 建立 bxCAN 基础通信与实时收发模型 | 待开始 |
| M11 | 建立 UART 或 RS485 Bootloader / IAP 基线 | 待开始 |
| M12 | 建立 Watchdog、Reset Cause 与 Fault Diagnostics 可靠性基础 | 待开始 |
| M13 | 打通 STM32 到上位机及数据库的基础链路 | 待开始 |
| M14 | 完成系统集成、异常注入与稳定性验证 | 待开始 |
| M15 | 完成工程收尾、演示和项目表达材料 | 待开始 |

## 已完成历史的事实校准

- M0～M3 均已完成，不因本次扩展路线而重新打开或修改历史结论。
- M2 已实际验证 FreeRTOS 启动、任务、阻塞、优先级抢占、同优先级时间片、Task Stack、FreeRTOS Heap，以及 TIM6 作为 HAL Timebase、SysTick 作为 FreeRTOS Kernel Tick 的分工。
- M3 已实际实现并验证同步 UART Logger、DEBUG/INFO/WARN/ERROR 等级、日志错误码、日志宏、`snprintf()` / `vsnprintf()` 格式化、CMSIS-RTOS2 Mutex 互斥、线程安全输出和编译期裁剪。
- M3 当前日志接口只用于 Task / Thread context，不是 ISR-safe API；异步日志、LogTask 和 UART DMA logging 不属于 M3 已完成范围。
- 当前仓库没有系统化 HardFault Diagnostic 实现。HardFault、Reset Cause 和故障现场保留统一放到 M12，不能写成已经完成。

## M0：工程准备（完成）

**Goal**

建立可复现、可追踪的工程起点和项目文档体系。

**Knowledge**

- STM32CubeMX、CMake、Ninja、GNU Arm Embedded Toolchain 的职责与调用关系。
- Git repository、working tree、staging area、commit、branch 和 remote 基础。
- 固件工程中源码、生成文件、构建产物与文档的边界。

**Implementation**

- 验证开发工具并记录版本。
- 建立 Git 仓库、`.gitignore`、README 和 docs 文档体系。
- 确定 CMake + Ninja + GNU Arm GCC 命令行构建路线。
- 建立远程仓库和稳定 Git 基线。

**Verification**

- 工具可执行性、仓库根目录、忽略规则和远程同步均有测试或 Git 历史记录。
- 首个稳定提交与项目管理文档已经形成。

**Deliverables**

- 可复现开发环境记录。
- CMake/Ninja/GNU Arm Toolchain 构建路线说明。
- Git 基线、README 和项目文档体系。

## M1：STM32 裸机稳定基线（完成）

**Goal**

在不引入 FreeRTOS 的前提下建立可构建、可下载、可运行、可观测和可重复启动的 Known Good Baseline。

**Knowledge**

- STM32F407VGT6 器件与封装选择。
- 系统时钟、GPIO、UART、下载与调试基本链路。
- 复位和完全断电重启的区别。

**Implementation**

- 配置 8 MHz HSE、168 MHz SYSCLK、APB1 42 MHz、APB2 84 MHz。
- 建立 PB2 LED heartbeat 和 USART2 TX 调试输出。
- 建立 CMake + Ninja + GNU Arm GCC 构建及 ST-LINK 下载链路。

**Verification**

- 固件完成真实 STM32F407VGT6 板级下载与运行。
- PB2 heartbeat、USART2 TX、NRST ×3 和完全断电重新上电 ×3 均已有通过记录。

**Deliverables**

- STM32 裸机 Known Good Baseline。
- 硬件基线、构建记录和板级测试记录。

## M2：FreeRTOS 最小系统与调度基础（完成）

**Goal**

在 M1 基线上启动 FreeRTOS，并理解最小多任务系统的调度和资源行为。

**Knowledge**

- Task、Scheduler、Priority、Ready/Running/Blocked 状态和 `osDelay()`。
- 抢占、同优先级时间片、Task Stack 和 FreeRTOS Heap。
- HAL Tick 与 FreeRTOS Tick 的职责边界。

**Implementation**

- 使用 CMSIS-RTOS2 接口建立 HeartBeatTask 和 UARTTask。
- TIM6 提供 HAL 1 ms Timebase，SysTick 用于 FreeRTOS Kernel Tick。
- 保留 Stack High Water Mark 与 FreeRTOS Heap 运行时观测。

**Verification**

- 单任务、双任务、优先级抢占和同优先级时间片实验均有板级记录。
- M2 Final Baseline 已通过构建、烧录、NRST ×3 和完全断电重新上电 ×3。
- Task Stack 与 FreeRTOS Heap 有真实测量数据。

**Deliverables**

- 稳定的 FreeRTOS 双任务基线。
- 调度、Stack、Heap 和时基关系测试记录。

## M3：线程安全、可裁剪的调试基础设施（完成）

**Goal**

建立统一、线程安全、可按编译配置裁剪的同步 UART 调试输出能力。

**Knowledge**

- 共享 UART 和共享格式化缓冲区的竞争风险。
- Mutex 的资源保护语义和同步阻塞日志的成本。
- 预处理器裁剪、Log Level、可变参数与格式化输出。

**Implementation**

- 建立独立 `debug_log` 模块，通过初始化参数注入 UART Handle。
- 提供 DEBUG、INFO、WARN、ERROR 四级日志宏和 `DebugLogStatus_t` 错误码。
- 使用 `snprintf()` / `vsnprintf()` 和 128 B static buffer 形成日志文本。
- 使用 CMSIS-RTOS2 Mutex 保护“格式化 + UART 阻塞发送”完整事务。
- 通过 `DEBUG_LOG_ENABLE` 和 `DEBUG_LOG_LEVEL` 进行编译期裁剪。

**Verification**

- 日志全局开启/关闭 clean build、四级过滤和双任务高频长日志压力测试均有记录。
- M3 Final Baseline 已重新测量 Flash、RAM、Task Stack 和 FreeRTOS Heap。
- NRST ×3、完全断电重新上电 ×3 通过；当前测试中未观察到日志交叉、乱码、Hang 或 HardFault。

**Deliverables**

- `Inc/debug_log.h`、`Src/debug_log.c` 和统一日志调用路径。
- M3 功能 checkpoint、测试记录、决策记录和资源成本记录。

## M4：RTOS 通信、同步和数据缓冲（完成）

**最新里程碑校准（2026-10-28）**

- M4.1 Queue、M4.2 Task Thread Flags、M4.3 Binary Semaphore / Mutex、M4.4 Ring Buffer Basic 和 M4.5 TIM7 ISR → Task 均完成历史板级实验；`adf5d5e` 为 ISR 功能提交。
- 2026-10-27 独立 Task ↔ Task SPSC 专项传输 50,000 个 `uint32_t` 全部 PASS；Full=7064、Empty=7419，`overflow=7064`，7501 OS ticks；有效容量 7 元素 / 28 B，不等于 DMA 并发已经验证。
- SPSC 功能提交 `d93665e` 经 PR #4 合并（`ccbd176`），M4 关闭文档 `3a8fa13`，经 PR #5 合并 main（`e793039`）；**M4 已正式合并并关闭**。
- M4 的实验任务和任务间对象是历史学习与验证资源，2026-10-28 M5-P0 已将其从默认固件清理，历史提交仍可追溯。
- DMA Buffer Ownership、ISR ↔ Task 数据通道、多生产者并发与高负载最坏延迟仍属 M5 后续未验证范围。

**Goal**

建立后续采集和通信模块所需的最小并发基础，能够根据数据、事件、同步和共享资源的不同语义选择正确机制。

**Knowledge**

- Queue：传递带所有权边界的结构化数据。
- Task Notification：轻量事件、计数或信号。
- Mutex：保护共享资源并理解其所有者语义。
- Binary Semaphore：任务/中断之间的同步，不用于表达 Mutex 所有权。
- Ring Buffer：缓存连续流式数据；当前 Task ↔ Task 单生产者/单消费者并发基线已验证。
- 生产者/消费者、数据所有权、ISR 与 Task 边界、`FromISR` API 和临界区基本使用。

**Implementation**

- 用两个或以上 Task 建立结构化数据的 Queue 生产者/消费者实验。
- 建立一次 Task/Task 或 ISR/Task 的 Task Notification 实验。
- 建立 Binary Semaphore 同步小实验，并与已有日志 Mutex 用途对比。
- 实现边界清晰、无动态内存依赖的基础 Ring Buffer，并定义满、空和读写返回行为。
- ISR 仅完成最小状态记录或通知，耗时处理留在 Task context。

**Verification**

1. 两个或以上 Task 能通过 Queue 正确传递结构化数据。
2. Task Notification 实验能稳定完成一次 Task/Task 或 ISR/Task 通知链路。
3. 能结合实际代码解释 Mutex 与 Binary Semaphore 的差异。
4. Ring Buffer 的基本读写、满、空、索引回绕和边界处理通过测试；扩展的 Task ↔ Task SPSC 50,000 元素并发专项已通过。
5. ISR 中没有耗时日志、复杂计算或阻塞 API。
6. 形成构建、运行、异常边界和板级测试记录。

**Deliverables**

- 最小 Queue、Notification、Semaphore 和 Ring Buffer 实验代码；可选构建的 `m4_spsc_test` 专项压力测试代码。
- 并发机制选择说明、数据所有权说明和 M4 测试记录；SPSC 原子内存序、可见性和 Full 重试计数语义记录。

## M5：ADC + Timer + DMA 数据采集子系统（当前）

**当前进度校准（2026-10-28）**

- M5-P0 运行时任务与日志清理已完成 Debug 增量构建、30 秒 UART 与 Heap/Stack 观测、NRST ×3、Power-cycle ×3 板级回归；仅保留 HeartBeat 和 UART 两个默认应用任务。
- 清理前后 UART 30 秒 882 → 9 行；FreeRTOS Free Heap 3432 → 13504 B；尚未形成 M5-P0 功能与文档的新 Commit / Push。
- M5-P1 开始之前须完成 Git 归档与 ADC/Timer/DMA 资源分配表。**M5 的 ADC、Timer 触发、DMA 和采集任务均尚未实现。**


**Goal**

建立项目第一条真实、连续、可测量的数据生产链路。

**Knowledge**

- ADC Regular Conversion、多通道扫描和采样时间。
- Timer Trigger、DMA Circular Mode、Half Transfer 和 Transfer Complete。
- ISR 到 Task 的最小通知、双半缓冲区所有权和简单平均/滤波。

**Implementation**

推荐数据链：

```text
Timer
  → ADC
  → DMA Circular Buffer
  → Half / Full Complete Callback
  → Task Notification
  → Acquisition Task
  → Queue
  → Processing Task
```

- 具体 ADC 引脚、通道、Timer 和采样参数必须在进入 M5 后根据开发板资源与冲突检查确定，本路线图不预设。
- 最终采集链路不使用阻塞式轮询。
- ISR 只发送最小通知，数据处理在 Task context 完成。

**Verification**

1. 最终采集链路不使用阻塞式轮询。
2. Timer 能按确认后的频率定时触发 ADC。
3. DMA 能自动、循环搬运采样结果。
4. Half Transfer 和 Transfer Complete 回调均能按预期触发。
5. ISR 只完成最小通知，不做耗时日志、滤波或复杂计算。
6. 实际采集、滤波或平均处理在 Task context 完成。
7. 能输出并核对采样频率、通道数、DMA Buffer 大小和处理周期，并处理半缓冲区覆盖与 Queue 满边界。
8. 完成真实板级采样测试，并保留原始数据或日志证据。

**Deliverables**

- ADC + Timer + DMA 采集模块与任务链路。
- 参数说明、数据流图和板级测试记录。

## M6：UART DMA + IDLE 不定长通信

**Goal**

建立不依赖逐字节阻塞处理的 UART 不定长收发链路，为后续 RS485 和协议处理提供稳定入口。

**Knowledge**

- UART DMA RX/TX、IDLE Line、DMA Buffer 和 Ring Buffer。
- 不定长帧边界、接收重启、缓冲区溢出和异常恢复。

**Implementation**

```text
UART
  → DMA
  → IDLE Event
  → Ring Buffer
  → Protocol Task
```

- 使用 HAL 标准接口与回调，不修改 HAL 库源码。
- 回调中只转移必要状态或数据并唤醒任务，解析留在 Protocol Task。

**Verification**

- 不同长度、连续到达、边界长度和异常输入均有测试。
- 验证 DMA 接收重启、Ring Buffer 满策略、数据顺序和丢包可观测性。
- 验证 UART DMA TX 完成与超时/错误处理。

**Deliverables**

- UART DMA + IDLE RX/TX 模块。
- 接收状态机、缓冲策略和测试记录。

## M7：RS485 半双工通信

**Goal**

在 M6 UART 通信基础上建立稳定的 RS485 物理层和半双工方向控制。

**Knowledge**

- A/B 差分线、DE/RE、Half Duplex、终端匹配、偏置、超时和总线冲突基本概念。
- UART 发送完成与 DE/RE 切换时序。

**Implementation**

- 在硬件资源和收发器型号确认后，通过 HAL GPIO + UART 实现方向控制。
- 将物理层收发与上层协议解析分离。
- 不在路线图中假定具体 DE/RE 引脚或额外硬件已经购买。

**Verification**

- 使用真实 RS485 节点验证双向半双工通信。
- 检查方向切换、超时、断线、连续帧和异常恢复。
- 必要时用逻辑分析仪或示波器验证时序和物理波形。

**Deliverables**

- RS485 HAL 驱动封装、接线记录、时序说明和板级测试记录。

## M8：Modbus RTU

**Goal**

在已验证的 RS485 链路上实现最小、可测试的 Modbus RTU 从站或主站能力，并至少完成一组基础寄存器访问。

**Knowledge**

- Address、Function Code、Data、CRC16、帧间隔、Timeout、Exception Frame 和 Register Model。

**Implementation**

- 按已确认角色实现最小功能码集合和寄存器映射。
- 将协议解析、业务数据和 RS485 物理层解耦。
- 对长度、地址、功能码、CRC 和超时逐层校验并返回明确错误。

**Verification**

- 正常读写、CRC Error、非法地址、非法功能码、长度异常和超时均有测试。
- 至少一组真实 Modbus 工具或节点互操作验证通过。

**Deliverables**

- Modbus RTU 最小实现、寄存器表、协议测试记录和异常案例。

## M9：统一数据模型

**Goal**

建立独立于 UART、Modbus 和 CAN 的业务数据结构，使同一份数据能够被不同接口消费。

**Knowledge**

- 数据模型与传输协议分层。
- Validity、Timestamp/Tick、版本、量纲、状态和并发访问边界。

**Implementation**

- 建立 Sensor Data、Device Status、Validity、Timestamp/Tick 和 Device Information 等最小模型。
- 定义采集任务、处理任务和协议任务之间的数据所有权与快照策略。
- JSON 只在上位机链路确有需要时加入，不把它作为内部数据模型。

**Verification**

- ADC 数据能够映射到统一模型。
- Modbus 与后续 CAN 能从同一业务模型读取或更新数据，不各自维护互相漂移的副本。
- 验证无效值、过期数据、时间回绕和并发访问边界。

**Deliverables**

- 统一数据结构、字段语义、所有权说明和映射测试。

## M10：CAN 基础通信

**Goal**

掌握 STM32F407 bxCAN、CAN 2.0 和实时消息收发模型；目标不是实现汽车上层协议。

**Knowledge**

- Standard/Extended ID、Data Frame、DLC、Arbitration、ACK、CRC。
- Bit Timing、Baud Rate、Filter、Tx Mailbox、Rx FIFO 和 CAN Interrupt。
- Error Active、Error Passive、Bus Off 和 Automatic Retransmission。

**Implementation**

推荐架构：

```text
CAN RX ISR → RTOS Queue → CAN Processing Task
Application Data → CAN TX Queue → CAN TX Task
```

- 使用 STM32 HAL CAN API，不提前写死 CAN 引脚、时钟参数或收发器型号。
- 先用 Internal Loopback 缩小软件配置问题，再在硬件条件确认后进行真实节点通信。
- 不在本里程碑实现 CAN FD、UDS、J1939 或 CANopen。

**Verification**

1. Internal Loopback 可作为早期配置验证手段。
2. 最终版本优先完成两个真实 CAN 节点之间的通信。
3. 验证 Filter、Tx Mailbox、Rx FIFO、Queue 满和消息顺序。
4. 能采集并分析 CAN Error 状态，至少覆盖错误计数与 Bus Off 观察。
5. 能解释 CAN 与 RS485/Modbus 的仲裁、拓扑、帧模型和错误处理差异。
6. 当前阶段不以 CAN FD、UDS、J1939 或 CANopen 作为验收项。

**Deliverables**

- CAN RX/TX 任务链路、消息映射、错误状态记录和测试报告。

## M11：Bootloader / IAP

**Goal**

理解 MCU 上电启动、Flash 分区、固件校验和 Application 跳转机制，建立第一版可恢复的串行升级链路。

**Knowledge**

- STM32 Flash Layout、Bootloader/Application Region 和 Linker Script。
- Vector Table、MSP、Reset_Handler、`SCB->VTOR` 和 Jump To Application。
- Flash Erase/Program、Firmware Length、Firmware Version 和 CRC32。

**Implementation**

第一版在进入 M11 后从 UART 或 RS485 中选择已经验证、风险更低的传输接口：

```text
Power On
  → Bootloader
  → 判断升级条件
  → 接收 Firmware
  → 写入 Flash
  → CRC32 校验
  → Jump APP
```

- Flash 分区地址、Application Offset 和升级触发条件必须依据实际固件体积、芯片 Flash 扇区和链接结果确定，本路线图不预设。
- 可在不增加当前复杂度的前提下抽象底层传输接口，为未来 CAN Bootloader 留扩展点，但不强制实现。
- 第一版不包含 Secure Boot、Encryption、Digital Signature、Dual Bank OTA、A/B Firmware 或网络 OTA。

**Verification**

- 验证正常升级、CRC 错误、长度越界、中途断开、无有效 Application 和重复上电场景。
- 验证 MSP、Reset Vector 和 VTOR 合法性检查。
- Bootloader 与 Application 均能独立构建，Memory Map 与固件边界可核对。

**Deliverables**

- Bootloader/Application 工程边界、链接配置、升级协议、Memory Map、Boot Process 和测试记录。

## M12：Watchdog、Reset Cause 与 Fault Diagnostics

**Goal**

建立设备异常检测、故障现场记录和受控恢复的可靠性基础。

**Knowledge**

- IWDG；WWDG 仅在实际需求出现时进一步研究。
- Supervisor Task、Task Heartbeat 和健康条件聚合。
- POR/PDR、PIN、Software、IWDG、WWDG、Brownout 等可获得的 Reset Cause。
- HardFault 时的 PC、LR、xPSR、MSP/PSP、CFSR、HFSR、BFAR 和 MMFAR。

**Implementation**

- 由关键任务上报 heartbeat，Supervisor 仅在所有关键健康条件满足时调用 `HAL_IWDG_Refresh()`。
- 禁止把喂狗分散成任意任务都可直接调用的无条件操作。
- 启动阶段读取并记录 Reset Cause，随后按 HAL 语义清理标志。
- 实现最小 HardFault Diagnostic，尽可能保留故障上下文；若实际条件允许，再设计重启后读取上次 Fault 信息。

**Verification**

- 正常运行不会误复位。
- 人为停止关键任务时，Supervisor 能拒绝喂狗并产生可解释的 IWDG Reset。
- Software Reset、PIN Reset、IWDG Reset 等可实施场景能够被区分和记录。
- 使用受控故障实验验证 HardFault 现场字段，不把单纯死循环视为诊断完成。

**Deliverables**

- Supervisor/Heartbeat 设计、Reset Cause 记录、HardFault Diagnostic、Fault Handling Flow 和故障注入测试。

## M13：上位机与数据库链路

**Goal**

打通 STM32 到 PC、Node-RED 和 InfluxDB 的基础数据链路，证明嵌入式数据能够被接收、解析和存储。

**Knowledge**

- 串行/总线数据到上位机消息的边界。
- Node-RED 流程与 InfluxDB 基础数据模型。
- 时间戳、重连、重复数据和数据有效性。

**Implementation**

```text
STM32 → 已验证通信接口 → PC → Node-RED → InfluxDB
```

- 复用 M9 统一数据模型。
- 只完成数据链路和必要的观察界面，不扩展复杂 Web 前端或云服务。

**Verification**

- 连续数据能够正确到达、解析并写入数据库。
- 断连、重连、异常字段和重复消息行为可观察。
- MCU 侧值、链路消息与数据库记录能够抽样对齐。

**Deliverables**

- Node-RED Flow、InfluxDB Schema、链路配置和端到端测试记录。

## M14：系统集成、异常与稳定性验证

**Goal**

在不继续堆叠新功能的前提下，回答系统发生异常后能否检测、记录、隔离并恢复。

**Knowledge**

- 长时间运行、压力测试、异常注入、资源水位和恢复判据。
- Stack、Heap、Queue、DMA、通信错误和 Watchdog 的关联分析。

**Implementation**

- 设计 8 h、12 h，条件允许时 24 h 的连续运行测试。
- 人为制造 UART Timeout、Modbus CRC Error、RS485 断线、CAN Bus Error、Queue 满、数据异常、Task 停止响应和 Watchdog Reset。
- 观察 Stack High Water Mark、Heap、Queue Usage、DMA 状态；CPU Load 仅在项目条件允许时加入。

**Verification**

- 每个异常都有预期行为、实际行为、日志证据和恢复结果。
- 重复异常不会造成不可解释的资源持续下降或永久失联。
- 对不能自动恢复的故障，明确隔离范围、人工恢复步骤和剩余风险。

**Deliverables**

- 集成测试矩阵、长稳报告、异常注入记录、资源趋势和剩余风险清单。

## M15：工程收尾与项目表达

**Goal**

把经过验证的工程事实整理成可复现、可演示、可解释的完整项目材料。

**Knowledge**

- 技术文档、架构表达、测试证据和工程复盘。
- 从需求、取舍、失败、验证到结果的项目叙事。

**Implementation**

- 整理 README、Architecture Diagram、Data Flow Diagram、Task Diagram、Communication Diagram、Memory Map、Boot Process、Fault Handling Flow、Test Report、Demo、Screenshots、Git History、Key Decisions、Error Log 和 Knowledge Gaps。
- 整理面试问答：FreeRTOS、Queue、Task Notification、DMA/中断、UART DMA + IDLE、RS485/Modbus、CAN、Bootloader、Watchdog、HardFault 和长时间死机分析。

**Verification**

- 新环境能按 README 构建关键固件。
- 演示步骤可重复，图示与当前代码一致。
- 每个核心设计都能用代码、测试或决策记录说明“为什么”。

**Deliverables**

- 完整项目文档、架构图、测试报告、演示材料、简历描述和面试问答。

## 强制学习顺序

```text
M4  RTOS 通信机制
 ↓
M5  ADC + Timer + DMA
 ↓
M6  UART DMA + IDLE
 ↓
M7  RS485
 ↓
M8  Modbus RTU
 ↓
M9  统一数据模型
 ↓
M10 CAN
 ↓
M11 Bootloader / IAP
 ↓
M12 Watchdog / Reset Cause / Fault Diagnostics
 ↓
M13～M15 系统集成与收尾
```

如果当前里程碑没有达到基本验收标准，不因为后续技术看起来更高级而跳过依赖阶段。UART 基础不稳定时不进入 Modbus；RS485 物理通信未验证时不调协议；并发边界未建立时不直接堆叠 ADC、DMA、CAN 或 Bootloader。

## Linux 辅助能力路线（不进入 STM32 里程碑编号）

Linux 在当前阶段是开发、构建、调试和上位机辅助能力，不是 STM32 项目的产品运行平台。未来两年按实际需要补充：

- Shell 与构建：Bash、GCC、GDB、Make、CMake、Ninja。
- 二进制基础：ELF、`objdump`、`objcopy`、`nm`、`readelf`。
- 用户态编程：Process、pthread、Mutex、Signal、File Descriptor、`open/read/write`。
- 系统观察：`/proc`、`/sys`、`/dev`、`strace`、`lsof`。
- 网络与远程：SSH/SCP、`ip`、`ss`、`ping`、`nc`、`curl`。

当前不把 Linux Kernel Source、Kernel Scheduler Source、Linux Driver 深入开发、Device Tree 深入、VFS 源码或内核内存管理源码加入本路线。需要转向 Embedded Linux / BSP 时再建立独立学习路线。

## 长期范围边界

- 固件外设层只使用 STM32 HAL；除非明确要求，不使用 SPL 或 LL。
- 不引入 AUTOSAR、复杂网络协议栈或与当前 MCU 主线无关的大型框架。
- 不把 CAN 扩张为 CAN FD、UDS、J1939 或 CANopen 项目。
- 不把第一版 Bootloader 扩张为安全启动、加密、签名、双 Bank 或网络 OTA。
- 不提前确定未验证的引脚、通道、Timer、Flash 分区或额外硬件。
- 所有“完成”必须有代码、构建、运行或测试证据；计划项不得写成已实现。
