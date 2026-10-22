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

- Binary Semaphore。
- Binary Semaphore 与 Mutex 的实际行为差异。
- Queue 传指针时的生命周期和所有权管理。
- ISR → Task Queue / Notification。
- ISR-safe 通信 API 和必要的高优先级任务唤醒行为。
- 多 Producer / 多 Consumer Queue 设计。
- Ring Buffer 与 Queue 的职责区别。

### 学习优先级

当前 Queue 基础已经完成。

Task → Task Thread Flags 基础已经通过 M4.2 实验验证，并单独记录在 KG-007。

下一优先级是 Binary Semaphore 与 Mutex 对比，然后进入 Ring Buffer，M4 后期再验证 ISR → Task 边界。

### 学习任务

- 建立 Binary Semaphore Task → Task 同步实验。
- 使用现有 Debug Logger Mutex 对比事件同步与资源所有权保护语义。
- 实现独立于 RTOS 的 Ring Buffer。
- 验证 Ring Buffer 满、空和回绕边界。
- M4 后期验证 ISR → Task Queue / Thread Flags 边界。
- 后续实际需要传递大型 Buffer 指针时再建立明确的生命周期和所有权规则。

### 状态

Queue 基础已掌握。

Thread Flags Task → Task 基础见 KG-007。

Binary Semaphore、Ring Buffer 和 ISR / Task 边界仍待验证。

## KG-007：CMSIS-RTOS2 Thread Flags、事件通知与重复事件合并

### 来源

M4.2 Task → Task Thread Flags 正常通知、重复 Flag bit 合并和 Final Baseline 稳定性验证过程。

### 当前理解

已经通过真实 STM32F407VGT6 验证 CMSIS-RTOS2 Thread Flags 的 Task → Task 基础行为。

当前已经理解：

- Thread Flags 是绑定在线程上的轻量事件状态机制，应用层不需要像 Message Queue 一样额外创建独立消息容器。
- `osThreadFlagsWait(..., osWaitForever)` 可以让 Work 在没有事件时进入 Blocked 状态。
- `osThreadFlagsSet()` 设置对应事件位后，等待该事件的 Work 能够恢复执行。
- 当前正常配置下，M4EventTask 每 1000 ms Set 一次 `M4_EVENT_WORK_READY`，M4WorkTask 能够稳定保持 Set Count 与 Receive Count 1:1。
- Thread Flags 不用于复制 `M4QueueMessage_t` 这类结构化数据。
- Queue 负责保存有限数量的独立消息，Thread Flags 更适合表达“某项工作待处理”。
- 同一个 Flag bit 在 Work 获得执行机会前被重复 Set 时不会累计多个独立事件。
- 已通过人工优先级实验稳定观察到：
  - 33 次相同 Flag Set。
  - Work 实际消费 11 次。
- 对同一个 bit 重复 Set 的本质是保持该 bit 为 Pending 状态，而不是建立 Event Count。
- 因此 Thread Flags 不能被直接当作无损事件计数器。
- 如果未来不同事件必须被区分，可以使用不同 Flag bit；如果同类事件的每一次发生都必须保存，则需要 Queue、Counting Semaphore 或其他具有计数 / 缓冲语义的机制。
- Debug Logger 输出顺序只能作为功能观测证据，不应该被当作 RTOS 内核精确调度 Trace。

当前 M4.2 Final Baseline：

- M4EventTask：
  - Priority：`osPriorityNormal`
  - Stack：1024 B
  - Period：1000 ms
- M4WorkTask：
  - Priority：`osPriorityNormal`
  - Stack：1024 B
- Event Flag：
  - `M4_EVENT_WORK_READY`
- Work：
  - `osThreadFlagsWait(..., osWaitForever)`
- Current Free Heap：8776 B。
- Minimum Ever Free Heap：8776 B。
- M4EventTask 历史最小剩余 Stack：528 B。
- M4WorkTask 历史最小剩余 Stack：528 B。
- NRST ×3：通过。
- Power-cycle ×3：通过。

### 缺少什么

尚未实际掌握和验证：

- ISR → Task Thread Flags。
- CMSIS-RTOS2 Thread Flags 在 ISR 场景中的具体限制和安全调用边界。
- 多个不同 Flag bit 同时 Pending 时的完整处理策略。
- `osFlagsWaitAny` 与更复杂 Flag 组合的工程应用。
- DMA Half Complete / Full Complete 使用不同 Flag bit 后的实际行为。
- 事件通知与 Buffer Ownership 同时存在时的最终数据采集架构。
- CMSIS-RTOS2 Thread Flags wrapper 与 FreeRTOS 底层 Notification 实现细节的更深入对应关系。

### 为什么重要

M5 ADC + Timer + DMA 和后续 UART DMA 都会产生“数据已经准备好”的异步事件。

如果把 Thread Flags 错误理解为事件计数器，当 Producer / ISR 产生事件的速度超过 Consumer Task 的处理速度时，可能错误地认为每一次事件都会被保留，从而造成不可见的数据丢失或 Buffer 状态错误。

必须明确区分：

- Event State：某项工作需要处理。
- Event Count：事件实际发生了多少次。
- Data：需要处理的数据内容。
- Buffer Ownership：当前哪一个执行上下文拥有对应 Buffer。

### 学习优先级

Task → Task Thread Flags 基础已经完成。

下一阶段优先完成 Binary Semaphore 与 Mutex 对比。

ISR → Task Thread Flags 和 DMA Buffer 事件设计在 M4 后期与 M5 数据采集阶段继续深化。

### 学习任务

- M4.3 建立 Binary Semaphore Give / Take 实验。
- 对比 Thread Flags、Binary Semaphore、Mutex 和 Queue 的职责。
- M4 后期设计一个最小 ISR → Task 通知实验。
- M5 ADC DMA 阶段分别考虑 Half Ready 和 Full Ready 事件。
- 在正式使用 DMA Buffer 前建立明确的数据状态与 Buffer Ownership 模型。

### 小实验

已完成：

- Task → Task 单 Flag 正常通知。
- `osThreadFlagsWait(..., osWaitForever)` 阻塞等待。
- 正常路径 11 Sets → 11 Receives。
- 相同 Flag bit 三连 Set Burst。
- 33 Sets → 11 Receives 事件合并验证。
- Event Producer 临时高优先级可重复实验。
- Thread Flags 与 Queue 并行运行。
- 新增 Task Stack 与 FreeRTOS Heap 重新测量。
- M4.2 Final Baseline NRST ×3。
- M4.2 Final Baseline Power-cycle ×3。

### 状态

Task → Task Thread Flags 基础已掌握。

相同 Flag bit 的事件合并语义已经通过真实硬件实验验证。

ISR → Task、多 Flag 和 DMA Buffer Ownership 仍待后续验证。

## KG-008：Binary Semaphore、Mutex、临界区与同步任务 Stack 余量

### 来源

M4 Binary Semaphore Task → Task 同步、Mutex 共享数据保护、Queue / Thread Flags / Semaphore / Mutex 联合运行和同步任务 Stack 调优过程。

### 当前理解

已经通过真实 STM32F407VGT6 验证 CMSIS-RTOS2 Binary Semaphore 和 Mutex 的基础行为。

当前已经理解：

- Binary Semaphore 可以用于 Task → Task 同步。
- `osSemaphoreNew(1, 0, ...)` 建立初始不可用的 Binary Semaphore。
- Consumer 使用 `osSemaphoreAcquire(..., osWaitForever)` 时，在 Semaphore 不可用期间进入阻塞等待。
- Producer `osSemaphoreRelease()` 后等待 Task 能够恢复执行。
- Binary Semaphore 当前表达同步信号，不负责携带结构化数据。
- Binary Semaphore 不应该被直接当作 Mutex 使用。
- Mutex 用于保护共享资源和 Read-Modify-Write 临界区。
- 两个 Task 在同一个 Mutex 下访问 `shared_value` 时能够保持数据更新连续。
- 临界区应该尽量短。
- 不应在 Mutex 持有期间加入无必要 Delay、复杂格式化或长时间阻塞 I/O。
- 将 Logger 放在 Mutex Release 后可以缩短临界区。
- Logger 位于临界区外后，UART 日志的打印顺序可能和真实临界区执行顺序不同。
- 因此日志顺序不能被直接视为 RTOS 调度 Trace。
- 在临界区内生成 `shared_op_seq` 可以更可靠地记录共享数据实际操作顺序。
- 按 `shared_op_seq` 检查时，前一次 after 与下一次 before 连续，可以作为当前互斥保护有效的观测证据。
- Queue、Thread Flags、Binary Semaphore 和 Mutex 当前职责可以区分为：
  - Queue：数据和独立消息。
  - Thread Flags：Pending Event State。
  - Binary Semaphore：同步信号。
  - Mutex：共享资源所有权与互斥。
- Task 能够长期运行不等于 Stack 配置安全。
- `osThreadGetStackSpace()` 的历史最小剩余空间必须用于检查最坏已执行路径。
- 本次新增同步 Task 初始最小余量：
  - Semaphore Task：32 B。
  - Mutex Task：8 B。
- 该余量虽然尚未出现可见 Stack Overflow，但不足以作为稳定 baseline。
- 调整后：
  - Semaphore Task 最小余量：288 B。
  - Mutex Task 最小余量：488 B / 520 B。
- 动态创建 Task 增加 Stack 后会直接减少 FreeRTOS Heap。
- 本次 Stack 总增量约 1536 B，Free Heap 同样减少约 1536 B，实际资源变化与配置一致。
- Current Free Heap 与 Minimum Ever Free Heap 长期保持一致时，可以说明当前观察窗口内未发生进一步的最坏 heap 消耗。

当前同步机制 Final 资源观察：

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

### 缺少什么

尚未实际掌握和验证：

- Counting Semaphore。
- Binary Semaphore 连续重复 Release 的更多边界。
- ISR → Task Semaphore 的 CMSIS-RTOS2 安全使用边界。
- Mutex Priority Inheritance 的实际行为。
- Priority Inversion 可重复实验。
- 多个 Mutex 同时存在时的 Lock Ordering。
- Deadlock 人工构造与诊断。
- Ring Buffer 与 Semaphore / Event 的组合模型。
- DMA Buffer Ownership 与同步机制的组合设计。

### 为什么重要

M5 ADC + Timer + DMA 和后续 UART DMA 会同时涉及：

- ISR 事件。
- Buffer Ready 状态。
- Task 唤醒。
- 数据所有权。
- 共享状态。
- 实时资源约束。

如果不能区分 Queue、Thread Flags、Semaphore 和 Mutex 的职责，可能出现：

- 把同步状态误当数据缓冲。
- 把 Mutex 误当事件通知。
- 事件丢失。
- 共享数据覆盖。
- 临界区过长。
- 优先级反转。
- 死锁。
- Stack 安全余量不足。

因此进入 M5 前需要先把 M4 的 Ring Buffer 和 ISR → Task 边界补齐。

### 学习优先级

Binary Semaphore 与 Mutex 基础已经完成。

下一优先级：

1. Ring Buffer。
2. ISR → Task 通信边界。
3. Buffer Ownership。
4. 再进入 M5 ADC + Timer + DMA。

Priority Inheritance、复杂 Deadlock 和 Counting Semaphore 在实际需求出现或 M4 剩余时间允许时继续深化，不作为进入 Ring Buffer 的前置阻塞条件。

### 学习任务

- 实现独立 Ring Buffer。
- 验证 Empty / Full / Wrap-around。
- 明确 Ring Buffer 单 Producer / 单 Consumer 基础假设。
- 建立最小 ISR → Task 通信实验。
- 核对 CMSIS-RTOS2 ISR-safe API。
- ISR 中保持非阻塞和最小执行路径。
- M5 前建立 ADC DMA Buffer Ownership 草案。
- 后续有明确优先级场景时构造 Mutex Priority Inversion / Inheritance 实验。

### 小实验

已完成：

- Binary Semaphore 初始 Depleted。
- Consumer `osWaitForever` Blocking Acquire。
- Producer 周期 Release。
- Waiting → Release → Acquired → Waiting。
- Binary Semaphore 与 Queue / Thread Flags 并行运行。
- 双 Task Mutex 共享数据保护。
- A `+1` / B `+10` Read-Modify-Write。
- 临界区外 Logger。
- `shared_op_seq` 临界区顺序观测。
- Queue + Thread Flags + Semaphore + Mutex 联合长期运行。
- Semaphore / Mutex Task Stack High Water Mark 测量。
- 初始 32 B / 8 B Stack 风险识别。
- Stack 调整后长期重新验证。
- FreeRTOS Heap 与 Stack 增量一致性验证。

### 状态

Binary Semaphore Task → Task 基础已掌握。

Mutex 共享数据基础互斥行为已通过真实硬件验证。

同步任务 Stack 资源调优方法已完成实机验证。

Ring Buffer、ISR → Task、Priority Inheritance 和 DMA Buffer Ownership 仍待后续验证。

## KG-009：Ring Buffer、FIFO、溢出统计与 ISR / Task 数据所有权

### 来源

M4 Ring Buffer 独立模块开发、C 指针错误修正、FreeRTOS 双 Task 确定性测试、溢出统计设计和真实 STM32F407VGT6 板级验证过程。

### 当前理解

已经通过实际编程和板级测试掌握 Ring Buffer 基础数据结构。

当前理解：

- Ring Buffer 使用固定容量数组循环存储数据。
- head 指向下一个写入位置。
- tail 指向下一个读取位置。
- head 和 tail 使用数组索引，而不是直接保存数据值。
- 使用预留一个空槽可以避免 Empty / Full 状态歧义。
- Empty 判断为 head == tail。
- Full 判断为 (head + 1U) % SIZE == tail。
- 物理数组长度为 8 时，有效容量为 7。
- 成功写入后才移动 head。
- 成功读取后才移动 tail。
- Full 写入失败不能破坏已有数据。
- Empty 读取失败不能修改输出数据。
- 对索引取模可以完成循环回绕。
- FIFO 表示先写入的数据先被读取。
- RingBufferStatus_t 用于区分成功、Full、Empty 和非法参数。
- 调用者必须检查 ReadByte / WriteByte 的返回状态。
- 不能将日志打印视为操作成功的证据。
- overflow_count 应记录实际发生的溢出写入事件。
- 不能将多次检查到 Full 直接等同于多次数据丢失。
- 当前 Full 策略为拒绝新数据，不覆盖旧数据。
- 覆盖旧数据和丢弃新数据适用于不同业务要求。
- Ring Buffer 只能吸收有限的短期数据积压。
- 如果长期生产速度大于消费速度，增加 Buffer 容量只能推迟溢出。
- 优化 Consumer 处理能力通常比单纯增加容量更重要。
- 数据丢失统计和告警对于工业采集系统具有实际意义。

已完成确定性板级测试：

```text
EMPTY       PASS
FULL        PASS
FULL READ   PASS
WRAP WRITE  PASS
WRAP READ   PASS
```

当前测试结果：

```text
overflow_count = 1
Final head     = 1
Final tail     = 1
```

当前能够解释为什么这些结果符合 Ring Buffer 的设计预期。

### 缺少什么

尚未实际掌握和验证：

- Ring Buffer SPSC 并发模型。
- 单 Producer / 单 Consumer 的索引写入所有权。
- ISR 与 Task 共享变量的原子访问条件。
- volatile 的作用和局限。
- 编译器优化与内存访问顺序。
- 必要的内存屏障和同步策略。
- ISR → Task **数据**交接过程（M4.5 已验证事件通知，尚未验证 Ring Buffer 数据共享）。
- CMSIS-RTOS2 更复杂 ISR-safe API 与高负载下事件合并、唤醒延迟的边界（当前已验证 Thread Flags 基线）。
- 多生产者 / 多消费者 Ring Buffer 的复杂性。
- 覆盖旧数据时对 tail 所有权的影响。
- 数据丢失告警和恢复状态机。
- DMA Half / Full Buffer Ownership。
- 采样数据批量搬运与消费任务处理能力预算。

### 为什么重要

M5 ADC + Timer + DMA 将持续产生采样数据。

后续 M6 UART DMA + IDLE 和其他通信模块也可能产生连续数据流。

如果 Ring Buffer 和同步边界设计错误，可能造成：

- 数据覆盖。
- 数据丢失。
- FIFO 顺序破坏。
- 重复读取。
- 读取尚未写入完成的数据。
- 读写索引竞争。
- 丢失中断事件。
- Consumer 无法及时处理数据。
- 实际故障被不准确的日志掩盖。
- 工业采集结果不可靠。

因此，M4 不仅需要 Ring Buffer 基础算法，还需要继续建立 ISR / Task 数据交接的正确边界。

### 学习优先级

Ring Buffer 基础数据结构已经完成。

下一优先级：

1. 已完成：TIM7 + CMSIS-RTOS2 ISR Thread Flags 通知基线。
2. 待深化：SPSC Ring Buffer 数据所有权。
3. 待深化：原子性、volatile 与内存可见性。
4. 待验证：ISR / Task 并发数据一致性。
5. 后续：ADC DMA Buffer Ownership。
6. 后续：数据丢失统计与告警策略。

### 学习任务

- 复习当前 RingBuffer_t 的 head / tail 所有权。
- 研究 ISR 与 Task 访问共享状态的约束。
- 阅读 CMSIS-RTOS2 中断上下文 API 规则。
- 已建立 TIM7 中断通知任务实验；后续补充高负载、并发数据访问验证。
- 验证 Task 阻塞等待与唤醒。
- 避免在 ISR 中使用当前同步 Logger。
- 建立可重复的数据顺序和完整性验证。
- 根据实验结果决定 Ring Buffer 是否需要增加同步机制。
- 为后续 DMA 数据缓冲建立独立所有权规则。

### 小实验

已完成：

- Ring Buffer 初始化。
- Empty 判断。
- Full 判断。
- Full 写入失败。
- Full 后已有数据保留。
- FIFO 数据读取。
- Read Index 回绕。
- Write Index 回绕。
- Empty 再次读取。
- overflow_count 增量验证。
- 双 Task Thread Flags 阶段同步测试。
- 五个测试阶段全部 PASS。
- Ring Buffer 测试期间 Stack / Heap 观测。
- TIM7 ISR → Worker Thread Flags 基础通知验证（`adf5d5e`，`irq=30/40/50/60` 对应 `received`，`errors=0`）。

待完成：

- ISR → Task 数据交接及高负载通知语义验证。
- SPSC 并发读写实验。
- 内存可见性与必要同步验证。
- 并发情况下的溢出统计正确性。
- M5 DMA Buffer Ownership 实验。

### 状态

Ring Buffer 基础原理已掌握。

Ring Buffer 单线程语义和串行协调的双 Task 确定性功能测试已经通过真实开发板验证。

Full / Empty / Wrap-around 和 FIFO 顺序已完成验收。

当前基础功能 checkpoint 为 `f8c3e52`；ISR → Task **事件通知** checkpoint 为 `adf5d5e`。

ISR / Task 并发安全、覆盖模式、DMA 数据所有权和正式工业级告警机制仍待后续学习与验证。


### 历史资料一致性待复核（2026-10-23）

当前文档历史基线将 Ring Buffer 描述为 `uint8_t`、物理 8 B / 有效 7 B；此前上传的工程快照 `Inc/ring_buffer.h` 却为 `uint32_t buffer[8]`，`WriteByte` / `ReadByte` 参数也为 `uint32_t`。这两个来源不一致。最终集成前必须基于**最新仓库 HEAD**核对实际类型和字节数；若确认为 `uint32_t`，应按“物理 8 个元素 / 32 B、有效 7 个元素 / 28 B”描述，并评估 API 命名。不得据旧文档自行假设最新版源码已改为 `uint8_t`。


## KG-010：TIM7 ISR → Task、NVIC 约束与事件合并

### 来源

2026-10-18～10-20 M4.5 TIM7 配置、FreeRTOS ISR API 核对及 STM32F407VGT6 板级通信实验。

### 当前理解

- TIM7 84 MHz 输入，PSC=8399、ARR=999，100 ms 更新事件。
- TIM6 HAL 时基的 `HAL_IncTick()` 仅由 TIM6 分支调用，TIM7 可独立接入既有 HAL 回调。
- TIM7 抢占优先级数值 5，在 4 位抢占优先级分组与 FreeRTOS Syscall IRQ 阈值数值 5 下允许执行相应 ISR-safe CMSIS-RTOS2 接口。
- ISR 做最小工作：累加统计、Set Thread Flag；不使用同步日志和阻塞 API。
- Worker 使用 `osThreadFlagsWait(osWaitForever)` 阻塞接收；Task 中进行非实时日志输出。
- 当前 `irq=30/40/50/60` 和 `received` 在采样点一致，`errors=0`；仅证明当时 10 Hz 负载下的链路行为。
- 相同 Flag bit 可合并，不能当成精确计数队列。

### 缺少什么

- 同一 Flag bit 被高频重复设置时的定量事件合并、吞吐量与处理延迟实测。
- 高负载下抢占调度与 ISR/Task 临界区的时序分析。
- IRQ 中断计数、Worker 接收计数与真实业务事件的严格一致性设计。
- 使用 Queue 或数据缓冲传送多次事件的更强语义与超限处理。

### 为什么重要

M5 ADC DMA 会产生连续事件；只会通知并不足以保证业务样本未丢失，必须同时设计数据所有权和消费能力。

### 学习优先级

M4.5 基础链路已完成；在 M4 Final Integration 完成后继续深入，高负载实验不阻塞本次功能 checkpoint。

### 学习任务

- 保留已验证 TIM7 100 ms 事件通知基线。
- 设计可观测的事件合并/过载实验，不在 ISR 打印日志。
- 比较 Thread Flags 事件状态与无损计数/Queue 的差异。
- 为 DMA Half/Full 的通知与数据交接制定不同的职责边界。

### 小实验

已完成：TIM7 100 ms、NVIC 中断优先级检查、Worker 阻塞/唤醒、低频通知统计、联合运行时资源采样。

待完成：高负载事件合并量化与通知延迟测量；SPSC 数据并发实验。

### 状态

基础功能 PASS，`adf5d5e` 已提交并推送；高负载和数据所有权边界未验证。

## KG-011：CubeMX 配置源、LF 规范与生成代码回归

### 来源

2026-10-18 多次 TIM7 代码生成引起 EOL-only 噪音以及 `TIM7.Period` 与 `tim.c` 暂时不同步的排查。

### 当前理解

- Git 索引 LF、工作区 CRLF、`.gitattributes` 要求 LF 可造成大量“已修改”噪音。
- `core.autocrlf=false` 不保证 CubeMX 输出始终 LF。
- 必须对照 `.ioc` 中配置与生成 C 文件值（例如 TIM7 ARR=999）。
- 使用定向字节级 CRLF → LF 可以避免无意重新编码中文注释。
- 代码生成后的 diff 应与功能范围一致，不能只根据构建成功判断源码未被覆盖。

### 缺少什么

- 更自动化且可重复的生成后 EOL 校验脚本与项目级 CI 规范。
- CubeMX 版本更新对自定义 USER CODE 和中间件源文件的兼容性回归。

### 为什么重要

消除无关 diff 可以提高工程可审计性，避免覆盖已验证的 M4 模块和引入无法解释的修改。

### 学习优先级

中。M4 Final Integration 前需复用已经验证的流程。

### 学习任务

生成前备份；生成后检查 `git status`、`git ls-files --eol`、`git diff --ignore-space-at-eol`、`git diff --check`，定向处理并核对构建结果。

### 小实验

已完成：本次 TIM7 生成后恢复至有限文件 diff，构建与板级试验通过。

### 状态

当前问题已解决；流程自动化和 CI 集成待后续阶段。
