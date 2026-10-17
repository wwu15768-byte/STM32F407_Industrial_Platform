# 决策记录

> 唯一回答：为什么这么设计？
> 更新时机：发生会影响后续实现的重要决策时。已生效的决策不静默改写；需要变化时新增决策并说明替代关系。

## DEC-20260906-01：确定仓库根目录

- 状态：已接受
- 决策：以 `STM32F407_Industrial_Platform/` 作为 Git 仓库根目录。
- 原因：使固件项目及其文档形成独立、边界清晰的版本库。
- 影响：外层 `My_Project/` 不纳入本仓库历史。

## DEC-20260906-02：采用命令行构建工具链

- 状态：已接受
- 决策：使用 STM32CubeMX 生成工程，CMake 配置构建，Ninja 执行构建，GNU Arm Toolchain 编译，STM32CubeProgrammer/ST-LINK 烧录与调试。
- 原因：构建流程清晰、可复现，减少对单一 IDE 工程文件的依赖。
- 影响：后续必须验证 CubeMX 生成结果、CMake 配置和命令行构建，而不能只验证 IDE 内构建。

## DEC-20260906-03：固件层只使用 STM32 HAL

- 状态：已接受
- 决策：STM32 外设代码必须使用 HAL；不使用 SPL，除非明确要求否则不使用 LL。
- 原因：统一硬件抽象方式，避免多套外设库混用造成维护和学习成本。
- 影响：不直接修改 HAL 源码，优先使用 HAL 标准回调与用户模块扩展。

## DEC-20260906-04：先建立裸机基线再引入 FreeRTOS

- 状态：已接受
- 决策：M1 先验证时钟、基础 GPIO、UART 和下载运行链路，M2 再引入 FreeRTOS。
- 原因：先建立 Known Good Baseline，可减少 RTOS bring-up 时的变量数量。
- 影响：M1 不实现正式 RTOS 业务任务。

## DEC-20260906-05：项目管理文档单一职责

- 状态：已接受
- 决策：路线、状态、周计划、每日事实、决策、错误、测试和知识缺口分别记录在独立文件中。
- 原因：避免同一事实散落和相互冲突，便于按更新时机维护。
- 影响：新增内容前先判断它回答哪个唯一问题。

## DEC-20260906-06：保留首次提交的原作者邮箱

- 状态：已接受
- 决策：不重写已推送的首次提交；该提交保留原邮箱，后续提交使用当前全局配置的 Gmail 地址。
- 原因：已有提交内容与作者身份有效，没有必要为了邮箱变化重写公开的 Git 历史。
- 影响：首次提交与后续提交的作者邮箱不同，但提交者名称保持 `wwu`；不执行 amend 或强制推送。

## DEC-20260908-01：以天空星 STM32F407VGT6 高配版作为硬件基线

- 状态：已接受
- 决策：项目目标 MCU 使用 STM32F407VGT6、LQFP-100；HSE 为 8 MHz，LSE 为 32.768 kHz；用户 LED 使用 PB2/BOOT1 且高电平点亮；调试串口使用 USART2 的 PD5/PD6；ST-LINK V2 通过 SWD 和 NRST 调试。
- 原因：以上配置已经通过开发板原理图、引脚分配图、厂商资料、MCU 数据手册和实物信息交叉核对。
- 影响：原先文档中的 STM32F407ZGT6 假设被本决策替代；CubeMX 必须选择 STM32F407VGT6，不得沿用 LQFP-144 器件配置。
- 供电约束：外部 12 V 必须先降压为稳定 5 V 后接入板端 5 V，设备共地，不同时由 USB 或 ST-LINK 供电。

## DEC-20260920-01：FreeRTOS 与 HAL 使用独立系统时基

- 状态：已接受
- 决策：进入 M2 后，将 STM32 HAL Timebase 从 SysTick 迁移到 TIM6；SysTick 专门用于 FreeRTOS Kernel Tick。
- 原因：FreeRTOS 需要 SysTick 支持任务延时、超时和调度时间基准；将 HAL 1 ms Tick 独立到 TIM6，可以避免 HAL 时基与 RTOS Tick 职责混杂，并保持 HAL 超时与时间函数正常工作。
- 影响：后续必须保留 TIM6 作为 HAL Timebase，不能将其随意分配给普通业务功能；SysTick、PendSV 和 SVC 的实现应以 FreeRTOS/CMSIS-RTOS2 port 为准，不手工重复定义。

## DEC-20260920-02：M2 采用 CMSIS-RTOS2 并分阶段建立调度基线

- 状态：已接受
- 决策：FreeRTOS 应用层接口使用 CMSIS-RTOS2；M2 按“单任务验证 → 双任务验证 → 优先级与资源验证 → 通信与同步”的顺序推进。
- 原因：分阶段引入功能可以控制变量数量，使每个 Git checkpoint 都对应一个经过真实硬件验证的工程状态；CMSIS-RTOS2 与当前 CubeMX 生成方式一致，并提供统一的任务管理接口。
- 影响：在完成基础调度、优先级、Task Stack 和 heap 验证前，不提前引入 Queue、Mutex、DMA、ADC、RS485 或 Modbus 等功能。

## DEC-20260927-01：M3 采用统一同步 UART 日志模块和 Mutex 保护策略

- 状态：已接受
- 决策：
    - 应用 Task 的调试输出统一通过 debug_log 模块和 LOG_DEBUG / LOG_INFO / LOG_WARN / LOG_ERROR 接口。
    - 日志模块通过初始化参数获取 UART Handle，使用静态共享日志 buffer，并使用 CMSIS-RTOS2 Mutex 保护“日志格式化 + UART 阻塞发送”的完整事务。
    - 日志全局开关和等级过滤采用编译期预处理裁剪。
    - M3 暂不引入独立 LogTask、Queue、DMA 或异步日志。
- 原因：
    - 多个 Task 直接调用同一个 UART 和共享格式化资源会产生共享资源竞争。
    - 统一日志入口可以隔离业务任务与底层 UART 实现，同时便于统一日志等级、错误处理、资源裁剪和后续替换输出后端。
    - M3 保持同步阻塞实现可以控制变量数量，在进入更复杂任务间通信和异步机制前先建立可验证的稳定基础。
- 影响：
    - 后续业务 Task 不应为了普通调试输出继续散落直接调用 HAL_UART_Transmit()。
    - 当前日志模块负责 UART 调试输出的格式化和互斥访问。
    - 当前 LOG_xxx() 仅用于正常 Task / Thread context，不作为 ISR-safe API。
    - 后续若引入异步 LogTask、Queue、UART DMA 或其他日志后端，应建立新的设计决策并说明对本决策的扩展或替代关系。
    - 新增重要日志调用路径后仍需重新检查 Task Stack、FreeRTOS Heap、Flash 和静态 RAM 成本。

## DEC-20260929-01：扩展为 M0～M15 的数据采集与多总线通信路线

- 状态：已接受
- 决策：
    - 项目长期定位调整为“基于 STM32F407VGT6 + FreeRTOS 的工业实时数据采集与多总线通信平台”。
    - M0～M3 已完成历史保持不变，M4 成为当前阶段。
    - M5 升级为 ADC + Timer + DMA 数据采集子系统。
    - 新增 M10 CAN、M11 Bootloader / IAP、M12 Watchdog / Reset Cause / Fault Diagnostics。
    - 原 M10 上位机链路、M11 系统联调、M12 项目收尾依次顺延为 M13、M14、M15。
    - 后续严格按 M4 → M5 → M6 → M7 → M8 → M9 → M10 → M11 → M12 → M13～M15 的依赖顺序推进。
- 原因：先建立 RTOS 数据传递和 ISR/Task 边界，再逐步增加实时采集、串行通信、多总线、升级和可靠性能力，能够控制变量数量，使每个阶段保持可运行、可验证和可解释。
- 影响：
    - 当前不实现 CAN、Bootloader、ADC DMA、Watchdog 或 HardFault Diagnostic，只记录后续目标、依赖和验收标准。
    - 不预设 ADC/CAN/RS485 引脚、采样通道、Timer、Flash 分区或未确认的外部硬件。
    - CAN 当前只规划 CAN 2.0 / bxCAN 基础，不加入 CAN FD、UDS、J1939 或 CANopen。
    - 第一版 Bootloader 不加入 Secure Boot、加密、数字签名、双 Bank、A/B Firmware 或网络 OTA。
    - Linux 只作为两年内逐步补充的开发与用户态基础能力，不进入 STM32 项目里程碑，也不加入 Linux Kernel 源码学习。

## DEC-20260930-01：M4 应用层 IPC 独立模块化并优先使用按值 Queue

- 状态：已接受
- 决策：
    - M4 Task 间通信与同步实验集中在独立 `m4_ipc` 应用模块中，不继续将具体通信逻辑堆入 CubeMX 生成的 `freertos.c`。
    - `freertos.c` 主要负责 RTOS 初始化入口和调用 `M4Ipc_Init()`。
    - M4 应用层 RTOS 对象优先使用 CMSIS-RTOS2 API 在用户模块中程序化创建。
    - 用户模块源码通过根目录 `CMakeLists.txt` 加入构建，不将用户源码长期维护在 CubeMX 生成的 `cmake/stm32cubemx/CMakeLists.txt` 中。
    - 第一版结构化 Queue 使用小型结构体按值传递，不使用指针消息。
- 原因：
    - 随着 Queue、Notification、Semaphore、Ring Buffer 以及后续采集任务增加，如果所有实现继续堆入 `freertos.c`，应用层职责会迅速混杂。
    - 独立模块能够保持 CubeMX 生成代码与用户业务逻辑边界清晰。
    - 程序化创建 RTOS 对象便于直接理解和控制 CMSIS-RTOS2 对象属性。
    - 第一版 Queue 使用按值结构体能够避免过早引入 Buffer 生命周期和指针所有权问题。
- 影响：
    - M4 后续 Notification 和 Semaphore 实验继续优先保持在应用模块边界内。
    - Ring Buffer 作为通用数据结构应进一步独立为单独模块，而不是与 RTOS IPC 实现耦合。
    - 后续如果 M5/M6 出现新的长期业务模块，可继续按照“生成代码负责硬件/RTOS入口，用户模块负责业务逻辑”的方式扩展。
    - 当确实需要传递大型 Buffer 指针时，必须重新建立明确的 Buffer 生命周期和所有权规则，不能直接套用当前按值 Queue 假设。

## DEC-20261016-01：M4 Ring Buffer 采用固定容量、预留空槽和满时拒绝写入策略

- 状态：已接受
- 决策：
    - M4 Ring Buffer 作为独立通用模块实现。
    - 模块文件为 `Inc/ring_buffer.h` 和 `Src/ring_buffer.c`。
    - Ring Buffer 核心实现不依赖 HAL 或 FreeRTOS。
    - 第一版使用固定容量静态数组，不使用动态内存。
    - 物理数组长度为 8 B，实际有效容量为 7 B。
    - 使用 head 和 tail 两个数组索引。
    - 使用预留一个空槽区分 Empty 和 Full。
    - Empty 时拒绝读取，不修改输出变量。
    - Full 时拒绝新数据写入，不覆盖已有数据。
    - Full 写入失败时增加 overflow_count。
    - 第一版使用单字节读写 API。
    - ISR / Task 并发安全不作为当前基础版本已完成的能力。
- 原因：
    - 固定容量和静态存储使内存使用边界明确。
    - 预留空槽可以减少状态管理复杂度。
    - 满时拒绝写入可以避免破坏已经缓存的数据。
    - overflow_count 使数据丢失具有可观测性。
    - 独立模块便于基础逻辑测试及后续业务复用。
    - ISR / Task 并发访问涉及额外的索引所有权和同步要求，需要单独验证。
- 验证：
    - Empty / Full / Full Read / Wrap Write / Wrap Read 均已通过真实板级测试。
    - overflow_count 在测试中从 0 增加至 1。
    - 最终 head=1、tail=1。
    - 功能 checkpoint 为 `f8c3e52`。
- 影响：
    - M4 Ring Buffer 基础功能可以作为后续 ISR → Task 实验的数据结构起点。
    - 后续不得仅因基础功能测试通过就假设其具备并发安全性。
    - 如果需要覆盖旧数据，必须重新分析 tail 所有权和并发访问策略。
    - 如果需要无损工业采样，应另行进行生产 / 消费吞吐量、缓存容量和数据丢失预算设计。
    - M5 ADC + Timer + DMA 不直接假设当前单字节 Ring Buffer 能够满足 DMA 数据块所有权要求。
    - 如果后续改变 Full 策略或并发模型，应新增设计决策并说明替代关系。
