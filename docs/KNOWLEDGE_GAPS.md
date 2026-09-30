# 知识缺口

> 唯一回答：我还有什么不会？
> 更新时机：发现或解决知识缺口时。只记录与当前或近期项目有关的内容。

## KG-001：Git 工作区、暂存区与提交快照

### 来源

M0 第一次稳定提交的分步练习。

### 当前理解

已能区分工作区、暂存区和提交快照，并能根据 `git status --short`、`git diff`、`git diff --staged` 和 `git log` 判断状态。已经实际完成 CubeMX 大批新文件的分组暂存、暂存快照审查、提交，并完成 feature branch 首次远程推送与 upstream 建立。

### 缺少什么

当前阶段核心基础已经掌握；后续需要继续实践 feature branch 合并、必要时的冲突处理，以及 main 稳定基线的更新流程。

### 为什么重要

只有能检查提交快照，才能避免把无关文件、生成物或错误内容写入历史。

### 学习优先级

当前基础已完成；后续按需巩固。

### 学习任务

后续提交继续保持“检查工作区 → 暂存 → 检查暂存快照 → 提交 → 检查历史”的流程。

### 小实验

已完成暂存前后差异比较、功能分支提交、main 与 feature branch 指针差异观察，并完成 `git push -u origin feature/m1-bare-metal-baseline` 建立远程跟踪关系。

### 状态

已掌握当前阶段所需基础。

## KG-002：CMake、Ninja 与 GNU Arm 驱动链的调用关系

### 来源

M0 开发工具链梳理。

### 当前理解

已经通过真实 M1 构建验证：CMake 根据工程配置建立构建规则，Ninja 执行具体任务，GNU Arm GCC 工具链完成 C 源码和启动文件处理、链接并生成目标固件。详细构建输出能够观察到 arm-none-eabi-gcc 的 MCU 宏、包含路径及实际编译命令。

### 缺少什么

尚未深入理解链接脚本各内存段、启动文件复位流程、链接 Map/size 信息，以及后续出现链接和内存问题时如何系统定位。

### 为什么重要

这是后续定位编译、链接和固件体积问题的基础。

### 学习优先级

当前基础验证已完成；后续出现链接、Flash/RAM 占用或启动问题时继续深化。

### 学习任务

M1 保留一次成功详细构建记录；后续在 FreeRTOS 或固件体积明显增长时再学习链接脚本、Map 文件和 Flash/RAM 使用分析。

### 小实验

已完成：使用详细构建输出观察实际 C 编译、启动文件处理、链接和固件产物生成过程。

### 状态

已完成当前阶段基础验证。

## KG-003：CubeMX 器件选择与 STM32F407 时钟树

### 来源

M1 最小裸机工程准备。

### 当前理解

已在实际 CubeMX 工程中选择 STM32F407VGT6、LQFP-100。当前系统使用 8 MHz HSE，经 PLL 得到 168 MHz SYSCLK/HCLK；APB1 为 42 MHz，APB2 为 84 MHz。PB2 已在真实目标板上完成 LED heartbeat 验证，USART2 PD5/TX 已通过 USB-TTL 完成真实发送验证。NRST 和完全断电重新上电均已重复验证。进入 M2 后，HAL Timebase 已从 SysTick 迁移到 TIM6，FreeRTOS 使用 SysTick 作为内核 Tick，当前双任务版本仍能稳定运行。

### 缺少什么

当前器件、时钟、基础 GPIO、USART2 TX 和启动链路已经完成当前阶段所需验证。USART2 RX 尚未验证；系统时钟精确频率尚未使用示波器或外部测量手段独立测量。

### 为什么重要

器件或时钟树配置错误会影响引脚、链接内存布局、HAL 时基和所有外设波特率。

### 学习优先级

当前阶段基础已完成。USART2 RX 留待正式通信阶段；精确时钟测量按后续调试需求开展。

### 学习任务

后续正式进入双向 UART 通信时验证 USART2 RX；如果出现波特率、定时或外设时钟相关问题，再结合示波器、逻辑分析仪和时钟树进行进一步测量。

### 小实验

已完成 STM32F407VGT6 裸机下载、PB2 LED heartbeat、USART2 TX、NRST ×3 和完全断电重新上电 ×3；进入 FreeRTOS 后上述 GPIO 和 UART 基础功能仍能在单任务和双任务系统中稳定运行。

### 状态

已掌握当前阶段所需基础。

## KG-004：FreeRTOS 调度、任务状态与运行时资源

### 来源

M2 FreeRTOS 单任务、双任务、优先级抢占、同优先级时间片以及运行时资源验证过程。

### 当前理解

已经能够区分 Running、Ready、Blocked 和 Idle Task 的基本关系。理解 `osDelay()` 会使当前用户任务进入 Blocked 状态，并允许其他 Ready Task 或 Idle Task 获得 CPU。

已经在真实 STM32F407VGT6 上分别完成：

- FreeRTOS 单用户任务调度验证。
- 两个周期任务稳定共存验证。
- 不同优先级任务抢占验证。
- 两个同优先级、持续 Ready Task 的时间片调度行为验证。
- Task Stack High Water Mark 运行时观测。
- FreeRTOS Current Free Heap 和 Minimum Ever Free Heap 运行时观测。

已确认当前工程：

- `configUSE_PREEMPTION = 1`
- `configUSE_TIME_SLICING = 1`
- SysTick 用于 FreeRTOS Kernel Tick。
- TIM6 用于 HAL Timebase。
- 高优先级任务从 Blocked 恢复为 Ready 后能够抢占低优先级 Running Task。
- 两个相同优先级且持续 Ready 的任务在启用时间片后均能持续获得 CPU 执行机会。
- Task 被切出后保存执行上下文，再次被调度时从原执行位置继续，而不是重新进入任务函数。
- `osThreadGetStackSpace()` 反映任务运行以来的历史最小剩余 Stack。
- FreeRTOS Heap 与链接脚本中的传统 C heap 属于不同内存管理概念。

当前 M2 Final Baseline 中：

- HeartBeatTask Stack：512 B，历史最小剩余约 384 B。
- UARTTask Stack：1024 B，历史最小剩余约 560 B。
- FreeRTOS Heap：15360 B。
- Current Free Heap：13592 B。
- Minimum Ever Free Heap：13592 B。

### 缺少什么

M2 当前阶段所需的基础调度和资源观测已经完成。

仍未深入掌握 PendSV 上下文保存/恢复的寄存器级完整实现，也尚未实际处理多个任务之间的数据传递、共享资源竞争和线程安全输出问题。

### 为什么重要

后续 ADC 采集、通信、数据处理和设备管理都会依赖多个任务。如果不能正确理解优先级、阻塞、时间片、Stack 和 Heap，就难以判断任务饿死、栈溢出、内存不足和实时性问题。

后续进入 Queue、Task Notification、Mutex 和日志模块后，还需要进一步理解数据所有权、竞争条件和同步策略。

### 学习优先级

当前 M2 基础已完成。

下一优先级转向 M3 的线程安全调试基础设施，以及 M4 的任务间通信与同步机制。

### 学习任务

M3 中结合多任务日志输出继续学习共享外设资源的线程安全策略。

M4 中通过 Queue、Task Notification、Mutex 和生产者/消费者实验进一步理解任务间通信、所有权和同步。

PendSV 寄存器级上下文切换过程后续按调试需求深化，不作为进入下一里程碑的阻塞条件。

### 小实验

已完成：

- 单任务 `osDelay()` 阻塞实验。
- 两个周期任务共存实验。
- 高低优先级抢占实验。
- 两个同优先级 Busy Task 时间片实验。
- Task Stack High Water Mark 实验。
- FreeRTOS Heap 当前与历史最低余量实验。
- M2 Final Baseline NRST ×3 和 Power-cycle ×3 验证。

### 状态

已掌握当前阶段所需基础。

## KG-005：线程安全日志、共享资源与编译期裁剪

### 来源

M3 统一 UART 调试日志、日志等级、编译期裁剪和双任务并发输出验证过程。

### 当前理解

已经能够识别多任务日志中的两个共享资源：

- USART2 外设。
- 静态共享日志 buffer。

理解 Mutex 不应只保护最终 HAL_UART_Transmit()，而应保护“共享 buffer 格式化 + UART 发送”的完整事务，否则一个任务可能在另一个任务发送前覆盖共享 buffer。

已经通过真实 STM32F407VGT6 完成两个 osPriorityNormal Task、约 10 ms 周期长日志并发压力实验；当前实验条件下未观察到半行交叉、payload 混合、乱码、Hang 或 HardFault。

已经理解：

- osMutexAcquire() timeout 使用 Kernel Tick 语义。
- Mutex 保证共享资源互斥，不保证任务按照固定顺序执行。
- 静态日志 buffer 不占用某一个 Task 的 Stack。
- snprintf() / vsnprintf() 等格式化调用仍会增加调用任务的历史峰值 Stack 使用。
- CMSIS-RTOS2 Mutex 会消耗 FreeRTOS Heap。
- C 预处理器不能直接使用 enum 完成 #if 日志等级判断，因此需要独立预处理数值宏。
- DEBUG_LOG_ENABLE 和 DEBUG_LOG_LEVEL 可以在编译阶段裁剪日志调用路径。
- 链接器会移除没有真实调用者的 section，因此日志资源成本应在真实业务调用存在后测量。

当前 M3 Final Baseline 已实测：

- HeartBeatTask 历史最小剩余 Stack：384 B。
- UARTTask 历史最小剩余 Stack：512 B。
- FreeRTOS Current Free Heap：13504 B。
- FreeRTOS Minimum Ever Free Heap：13504 B。

完整日志开启与关闭的构建差值约为：

- Flash：6108 B。
- 静态 RAM：544 B。
- 相比 M2，UARTTask 历史峰值 Stack 使用增加约 48 B。
- 相比 M2，FreeRTOS Heap 额外消耗约 88 B。

### 缺少什么

尚未掌握和验证：

- ISR-safe 日志设计。
- 异步 LogTask 和日志 Queue。
- UART DMA 日志发送。
- 日志高负载下的背压、丢弃和流量控制策略。
- Mutex 优先级继承和优先级反转的实际场景。
- 使用 linker Map 文件精确拆分 vsnprintf()、libc 和日志模块各自的 Flash / RAM 成本。
- 多个共享资源同时存在时的锁顺序和死锁规避。

### 为什么重要

后续 ADC 采集、通信协议、数据处理和设备管理会进一步增加任务数量与共享资源。如果不能正确处理资源所有权、临界区和阻塞行为，可能出现日志交叉、数据破坏、任务阻塞、优先级反转甚至死锁。

同时，嵌入式日志的格式化和同步机制会消耗 Flash、RAM、Task Stack 和 FreeRTOS Heap，需要在可观测性和资源成本之间进行工程取舍。

### 学习优先级

当前 M3 所需的同步 UART 日志基础已经掌握。

下一阶段优先进入 M4，通过 Queue、Task Notification、Ring Buffer 和生产者/消费者模型进一步学习任务间通信、数据所有权和同步。

ISR-safe 和异步日志在实际业务需求出现后再深化，不作为当前进入 M4 的阻塞条件。

### 学习任务

M4 中实际建立 Queue 数据传递实验。

对比 Queue、Task Notification 和 Mutex 的适用场景。

学习 Ring Buffer 的读写索引、容量、满/空判断和所有权。

建立生产者/消费者实验并观察任务阻塞和唤醒行为。

后续固件体积明显增长时使用 Map 文件分析日志格式化依赖。

出现 ISR 日志需求时单独设计 deferred logging，而不直接在 ISR 中调用当前 LOG_xxx()。

### 小实验

已完成：

- DEBUG_LOG_ENABLE = 1 / 0 clean build 对比。
- DEBUG / INFO / WARN / ERROR 四级过滤实板验证。
- 两个同优先级 Task 约 10 ms 高频长日志并发压力实验。
- Mutex 保护共享静态 buffer 和 USART2 输出实验。
- M3 Task Stack 与 FreeRTOS Heap 重新测量。
- M3 Final Baseline NRST ×3。
- M3 Final Baseline Power-cycle ×3。

### 状态

已掌握当前 M3 阶段所需基础。

## KG-006：FreeRTOS Queue、数据所有权与 Backpressure

### 来源

M4.1 Queue Producer / Consumer、Queue Full、Sequence Gap 和 Task Stack 压力验证过程。

### 当前理解

已经通过真实 STM32F407VGT6 验证 CMSIS-RTOS2 Message Queue 的基本 Task → Task 数据传递行为。

当前已经理解：

- Queue 可以在 Producer 和 Consumer 之间建立明确的数据交接边界。
- 当前 `M4QueueMessage_t` Queue 使用按值复制，`osMessageQueuePut()` 使用消息地址作为复制源，而不是将 Producer 局部变量指针保存进 Queue。
- Producer 后续修改自己的局部 `message` 不会修改已经进入 Queue 的数据副本。
- Consumer 使用 `osMessageQueueGet(..., osWaitForever)` 时，在 Queue 为空期间进入 Blocked，而不是持续 Busy Polling。
- Queue Put timeout = 0 时，Queue 满会立即返回失败，适合明确暴露 Backpressure。
- Queue Length 只能吸收有限时间的突发数据，不能解决长期 Producer Throughput 大于 Consumer Throughput。
- Queue Full 只会使新的 Put 失败，不会破坏此前已经成功进入 Queue 的旧消息。
- Consumer 不一定在第一次 Drop 时立即看到 Sequence Gap，因为 Queue 中可能仍有 Drop 发生前的连续旧库存。
- 当旧库存消费完成后，Drop 会表现为 `expected_sequence != actual_sequence`。
- Task Stack 必须根据最坏执行路径测量；正常路径余量充足不代表异常日志路径也安全。
- Queue Full 后的同步日志格式化路径会显著增加 Producer Stack 峰值。
- FreeRTOS 动态 Task Stack 会直接占用 FreeRTOS Heap。

当前 M4.1 Final Baseline：

- Queue Length：8。
- Queue Item：12 B。
- Producer Stack：1024 B。
- Consumer Stack：1024 B。
- Producer Period：200 ms。
- Put Timeout：0。
- Consumer Wait：`osWaitForever`。
- Current Free Heap：11048 B。
- Minimum Ever Free Heap：11048 B。

### 缺少什么

尚未实际掌握和验证：

- Task Notification / CMSIS-RTOS2 Thread Flags。
- Notification 的重复事件、bit flag 和计数行为。
- Binary Semaphore。
- Binary Semaphore 与 Mutex 的实际行为差异。
- Queue 传指针时的生命周期和所有权管理。
- ISR → Task Queue / Notification。
- `FromISR` API 和必要的高优先级任务唤醒行为。
- 多 Producer / 多 Consumer Queue 设计。
- Ring Buffer 与 Queue 的职责区别。

### 为什么重要

M5 ADC + Timer + DMA、M6 UART DMA 和后续通信模块都会持续产生数据和事件。

如果不能区分“结构化数据传递”、“轻量事件通知”、“同步”和“共享资源保护”，容易出现错误的 RTOS 对象选择、数据生命周期错误、任务长期阻塞或无界数据积压。

### 学习优先级

当前 Queue 基础已经完成。

下一优先级是 Task Notification / CMSIS-RTOS2 Thread Flags，然后是 Binary Semaphore 与 Mutex 对比，最后进入 Ring Buffer。

### 学习任务

- 建立最小 Task → Task Notification 实验。
- 比较 Queue 和 Notification 的资源与语义差异。
- 建立 Binary Semaphore 同步实验。
- 使用现有 Debug Logger Mutex 对比资源所有权语义。
- 实现独立于 RTOS 的 Ring Buffer。
- M4 后期再验证 ISR → Task 边界。

### 小实验

已完成：

- Queue Producer → Consumer 正常路径。
- 小结构体按值传递。
- Consumer Blocking Receive。
- Queue Full。
- Backpressure。
- Drop Counter。
- Sequence Gap。
- Producer Stack 正常路径与压力路径对比。
- Producer Stack 512 B → 1024 B 调整与复测。
- FreeRTOS Heap 重新测量。
- M4.1 Final Baseline 稳定性验证。

### 状态

Queue 基础已掌握。

Task Notification、Binary Semaphore、Ring Buffer 和 ISR / Task 边界仍待验证。
