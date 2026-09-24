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
