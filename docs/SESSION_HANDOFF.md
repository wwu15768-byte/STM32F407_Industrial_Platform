> 当前会话快照；会话切换前整体更新。
> 更新时间：2026-10-27。
> 最新已确认 Git 事实：`d93665e` 位于 `feature/m4-spsc-concurrency-verification` 并已推送 origin，用户确认工作区 clean；未收到 PR 已合并的结果。本 ZIP 只是文档附件，不代表 Git 已自动提交。
> 以下 M0～M4.5 的旧基线章节作为**历史验证快照**保留；如与本页「当前」字样冲突，以 2026-10-27 的当前状态和最新验证章节为准。

## 当前 Milestone

M4：RTOS 通信、同步和数据缓冲，功能验证基本齐备，但最终验收、合并及里程碑关闭仍未完成。M0～M3 已完成。

- M4.1 Queue：完成；M4.2 Task → Task Thread Flags：完成（含相同 Flag 合并）。
- M4.3 Binary Semaphore / Mutex：完成基础和联合回归。
- M4.4 Ring Buffer：Empty/Full/Wrap/FIFO 基础测试 PASS；Task ↔ Task SPSC 50,000 元素并发专项 PASS（独立分支 `d93665e`）。
- M4.5 TIM7 IRQ → Thread Flags → Worker：100 ms 基础链路、用户确认的持续运行和原模块回归 PASS（`adf5d5e`）。
- M4 Final Integration：临时同机联合实验和 SPSC 专项已通过；仍缺本次 NRST ×3 / Power-cycle ×3 的独立证据、测试 OFF 后正式固件回归、SPSC → M4 PR、合并后的验收及最终 docs checkpoint。
- M5：未开始，不提前创建采集业务分支。

## 当前 Branch 与 Git 基线

- 最新用户确认 Branch：`feature/m4-spsc-concurrency-verification`。
- 当前 HEAD / origin：`d93665e`：`feat: verify M4 ring buffer SPSC concurrency baseline`；用户确认工作区 clean。
- 父级 M4 Branch：`feature/m4-rtos-communication-baseline`，最后已知 HEAD / origin 为 `405b9ab`：`docs: record M4 ISR notification baseline verification`。
- M4.5 功能：`adf5d5e`；Semaphore 恢复：`c4c0721`；Ring Buffer 基础：`f8c3e52`。
- **尚无证据表明 SPSC → M4 PR 已经合并、M4 → main 已合并或 M4 里程碑已关闭。** 本文档压缩包的新文档提交号仍未产生。

## 2026-10-27 M4 Final Integration / SPSC 最新验证快照

- 旧实验（临时 `freertos.c`）：Ring Buffer 的 `EMPTY / FULL / FULL READ / WRAP WRITE / WRAP READ` 全部 PASS 后原 Semaphore Release/Acquire、Queue、Task Flags、Mutex、TIM7 IRQ 继续共同运行，用户确认长时间运行通过；此实验中读写按 Thread Flags 阶段分开，**不是**独立 SPSC 并发测试。
- 新专项：使用 C11 `_Atomic` `head/tail`、acquire/release 内存序、两个独立 Producer/Consumer Task，Ring Buffer 是 `uint32_t buffer[8]`，物理 32 B，有效 7 个元素 / 28 B。
- 板级实际日志：

```text
[I][SPSC] START total=50000 capacity=7 elements
[I][SPSC] PASS sent=50000 recv=50000 full=7064 empty=7419
[I][SPSC] overflow=7064 elapsedTicks=7501
[I][SYS] SpscPFree=876 B SpscCFree=444 B
[I][SYS] M4EvtFree=528 B M4WorkFree=528 B Heap=3432 B MinHeap=3432 B
[I][SYS] ISRWorkerFree=520 B
```

- 解释：`full=7064` 是写入失败后保留当前待发送值并重试的次数，和 `overflow=7064` 一致，**不是最终丢弃的数据个数**；`empty=7419` 证明确实出现空缓冲重试，`sent=recv=50000` 且程序完整序号校验 PASS。
- 同窗口其他 M4 模块继续正常：M4Q `seq=1..40` 连续、M4F Set/Receive、Semaphore Release/Acquire、Mutex +1/+10 共享值、TIM7 到 `irq=70 received=70 errors=0`。
- 本次 7501 OS ticks 约 7.5 s 的**单次专项压力测试**不等于 M4 Final Baseline 长期运行时长，系统级长期运行窗口与复位仍需独立证据。
- CMake `M4_ENABLE_SPSC_TEST` 默认 OFF，但 CMake Cache 可能保留之前 ON；关闭时必须显式 `-DM4_ENABLE_SPSC_TEST=OFF`，重新配置、Clean Build、烧录并确认无 `[SPSC] START`。
- 本轮证明的是 Task ↔ Task SPSC，不宣称 ISR ↔ Task Ring Buffer、DMA Ownership、MPSC / MPMC 或极限最坏延迟安全。

## 已完成并验证的内容

### M0～M3

- M0 开发环境、Git 和项目文档体系已完成。
- M1 STM32F407VGT6 裸机 Known Good Baseline 已完成真实板级验证。
- M2 FreeRTOS 启动、双任务、优先级抢占、同优先级时间片、Task Stack、FreeRTOS Heap 和复位稳定性验证已完成。
- HAL Timebase 使用 TIM6，SysTick 用于 FreeRTOS Kernel Tick。
- M3 已建立独立 `debug_log` 模块。
- 当前日志后端为 USART2，支持 DEBUG / INFO / WARN / ERROR。
- 使用 CMSIS-RTOS2 Mutex 保护共享日志 buffer 和 UART 阻塞发送完整事务。
- M3 已完成日志编译期裁剪、等级过滤、并发压力和 Final Baseline 验证。
- M3 已通过 PR 合并到 `main`，Merge Commit 为 `c13b8e5`。

### M4.1 Queue Communication Baseline

当前结构：

```text
M4ProducerTask
      |
      | M4QueueMessage_t
      v
M4DataQueue[8]
      |
      v
M4ConsumerTask
```

当前 Final Baseline：

- Queue Length：8。
- Queue Item：`M4QueueMessage_t`。
- Queue Item Size：12 B。
- Producer Priority：`osPriorityNormal`。
- Producer Stack：1024 B。
- Producer Period：200 ms。
- Queue Put Timeout：0。
- Consumer Priority：`osPriorityNormal`。
- Consumer Stack：1024 B。
- Consumer Queue Get Timeout：`osWaitForever`。

已验证：

- 结构体按值复制。
- Consumer Blocking Receive。
- Queue Full。
- Backpressure。
- Put Failure。
- Drop Counter。
- Sequence Gap。
- Queue 已入队数据不会被后续 Put Failure 破坏。
- Producer Stack 最坏路径测量。
- Producer Stack 从 512 B 调整为 1024 B。
- Final Baseline 稳定性验证。

### M4.2 Thread Flags Baseline

当前结构：

```text
M4EventTask
     |
     | osThreadFlagsSet()
     | M4_EVENT_WORK_READY
     v
M4WorkerTask
     |
     | osThreadFlagsWait(..., osWaitForever)
     v
Blocked -> Ready -> Run -> Wait
```

Final Baseline：

- Event Flag：`M4_EVENT_WORK_READY`。
- M4EventTask：
  - Priority：`osPriorityNormal`。
  - Stack：1024 B。
  - Period：1000 ms。
- M4WorkerTask：
  - Priority：`osPriorityNormal`。
  - Stack：1024 B。
  - Wait：`osThreadFlagsWait(..., osFlagsWaitAny, osWaitForever)`。

正常路径实际验证：

```text
Set Count     = 11
Receive Count = 11
```

即：

```text
11 Sets -> 11 Receives
```

已确认：

- Worker 在没有事件时阻塞等待。
- Event Producer Set Flag 后 Worker 能够正常被唤醒。
- Queue 与 Thread Flags 可以同时稳定运行。
- Thread Flags 用于事件通知，不携带当前 Queue 中的结构化消息内容。

### M4.2 重复 Flag bit 合并实验

临时实验配置：

- M4EventTask Priority：`osPriorityAboveNormal`。
- M4WorkerTask Priority：`osPriorityNormal`。
- 每轮 Burst 连续 Set 相同 `M4_EVENT_WORK_READY` bit 3 次。

实际观察：

```text
burst=1  total_set=3   receive=1
burst=5  total_set=15  receive=5
burst=10 total_set=30  receive=10
burst=11 total_set=33  receive=11
```

最终：

```text
33 Sets -> 11 Receives
```

已确认：

- 相同 Flag bit 被重复 Set 时不会像 Queue 一样形成多个排队元素。
- Worker 消费之前，相同 bit 的多次 Set 会合并为一个 Pending Flag 状态。
- Thread Flags 表达 Event State，不表达无损 Event Count。
- Thread Flags 不应该被直接当成事件计数器使用。
- 如果未来 ADC DMA Half / Full 事件需要区分，应使用不同事件状态或更加明确的数据 / Buffer Ownership 机制。

实验完成后已经删除 Burst 测试逻辑并恢复两个 Event Task 的正常优先级。

### M4.2 资源验证

Final Baseline：

```text
HeartFree=384 B
UARTFree=504 B
M4ProdFree=808 B
M4ConsFree=496 B
M4EvtFree=528 B
M4WorkFree=528 B
Heap=8776 B
MinHeap=8776 B
```

Burst 压力路径：

```text
M4EvtFree=520 B
M4WorkFree=528 B
Heap=8776 B
MinHeap=8776 B
```

已确认：

- Current Free Heap 和 Minimum Ever Free Heap 保持一致。
- 未观察到 Heap 持续下降。
- Burst 路径只使 M4EventTask 历史最小剩余 Stack 从 528 B 降至 520 B。
- 当前两个新增 1024 B Task 保持足够 Stack 余量。
- 当前不缩减 Event Task Stack，继续保留后续异常路径和实验余量。

### M4.2 Final Baseline

已经恢复：

- M4EventTask Priority = `osPriorityNormal`。
- M4WorkerTask Priority = `osPriorityNormal`。
- Event Producer 每 1000 ms 单次 Set。
- 删除 Burst 三连 Set。
- 原 M4 Queue 保持正常 baseline。

稳定性：

- NRST ×3：PASS。
- Power-cycle ×3：PASS。
- 六次重新启动后 Queue、Thread Flags、Heartbeat、UART 和资源监测均正常恢复。
- 未观察到 Queue Full。
- 未观察到 Sequence Gap。
- 未观察到 Heap 持续下降。
- 未观察到 Hang。
- 未观察到 HardFault。

M4.2 最新功能 checkpoint：

`<fd80cc9>`：`feat: establish RTOS thread flag notification baseline`

### M4.3 Binary Semaphore Baseline

结构：

```text
SemProducerTask
      |
      | osSemaphoreRelease()
      v
   SyncSem
      |
      | osSemaphoreAcquire(..., osWaitForever)
      v
SemConsumerTask
```

配置：

- `SyncSem`
  - Maximum Count = 1。
  - Initial Count = 0 / Depleted。
- Producer Period：约 2000 ms。
- Consumer：`osWaitForever` 阻塞等待。

已验证：

```text
Consumer Waiting
→ Producer Released
→ Consumer Acquired
→ Consumer Waiting
```

已确认：

- Count = 0 时 Consumer 阻塞。
- Producer Release 后 Consumer 被唤醒。
- Consumer Acquire 后重新等待。
- Queue 和 Thread Flags 同时保持正常。
- Binary Semaphore 用于任务同步，不作为共享资源所有权锁。

Checkpoint：

`435ba4f`：`feat: establish RTOS semaphore synchronization baseline`

### M4.3 Mutex 对比 Baseline

结构：

```text
MutexTaskA ─┐
            ├─ SharedDataMutex → shared_value
MutexTaskB ─┘
```

行为：

- MutexTaskA：`shared_value + 1`
- MutexTaskB：`shared_value + 10`
- Read-Modify-Write 位于 Mutex 临界区。
- Logger 位于 Mutex Release 后。
- 增加 `shared_op_seq`，在临界区内记录真实操作序号。

实际观察：

```text
seq=37 A value=198->199
seq=38 B value=199->209

seq=39 A value=209->210
seq=40 B value=210->220
```

已确认：

- `shared_op_seq` 连续。
- 前一次 after 与下一次 before 连续。
- 未观察到旧值覆盖。
- UART 日志打印顺序不等于 Mutex 临界区精确执行顺序。
- Mutex 用于共享资源互斥和所有权保护。

Checkpoint：

`c5bc54a`：`feat: establish RTOS mutex protection baseline`

### M4.3 Stack 与 Heap 调优

首次测量：

```text
SemProFree=32 B
SemConFree=32 B
MutexAFree=8 B
MutexBFree=8 B
Heap=6104 B
MinHeap=6104 B
```

判断同步实验 Task Stack 余量不足。

调整：

- Semaphore Task：约 512 B → 768 B。
- Mutex Task：约 512 B → 1024 B。

调整后：

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

Stack 总增量约 1536 B，Free Heap 同样减少约 1536 B。

已确认：

- Stack 余量恢复。
- Heap 变化与 Stack 配置一致。
- Current Free Heap 与 Minimum Ever Free Heap 长期一致。
- Queue、Thread Flags、Semaphore、Mutex 长期联合运行正常。
- 未观察到 HardFault、Hang 或 Heap 持续下降。

Checkpoint：

`c84fed8`：`fix: increase M4 synchronization task stack margins`

### M4.4 Ring Buffer Baseline（2026-10-16 历史测试快照）

> 本小节保留当时 Ring Buffer 测试阶段的事实与用语；之后 Semaphore 已恢复、M4.5 ISR 通知已验证。文件末尾的“当前最新验证情况”才是本次交接状态。

当前新增模块：

```text
Inc/ring_buffer.h
Src/ring_buffer.c
```

当前通过根目录 `CMakeLists.txt` 加入构建。

数据结构：

```text
RingBuffer_t
  |
  +-- buffer[8]
  +-- head
  +-- tail
  +-- overflow_count
```

当前设计：

- 数据类型：uint8_t。
- 物理数组长度：8 B。
- 实际有效容量：7 B。
- 不使用动态内存。
- 不依赖 HAL / FreeRTOS。
- 使用预留一个空槽的设计。
- Empty：head == tail。
- Full：(head + 1U) % RING_BUFF_SIZE == tail。
- Full 时拒绝新数据写入。
- Full 失败时保留已有数据。
- 每次 Full 写入失败增加 overflow_count。
- Empty 读取失败时不修改输出变量。

已有 API：

```text
RingBuffer_Init()
RingBuffer_WriteByte()
RingBuffer_ReadByte()
RingBuffer_IsEmpty()
RingBuffer_IsFull()
```

2026-10-16 本轮临时复用（现已通过 `c4c0721` 恢复 Semaphore）：

```text
SemProducerTask
       |
       | Thread Flags 阶段同步
       v
SemConsumerTask

两个 Task 分阶段访问同一个 ring_buffer
```

该测试结构只用于确定性基础功能验证。

已完成测试：

```text
EMPTY       PASS
FULL        PASS
FULL READ   PASS
WRAP WRITE  PASS
WRAP READ   PASS
```

实际结果：

```text
overflow_count = 1

WRAP WRITE:
head = 1
tail = 2

WRAP READ:
head = 1
tail = 1
```

已确认：

- Empty 判断正确。
- Full 判断正确。
- Full 写入失败不破坏已有数据。
- FIFO 顺序正确。
- Write Index 和 Read Index 回绕正确。
- Empty 读取失败不修改输出数据。
- 溢出计数在本次测试中符合预期。

本轮联合运行观察：

- Queue Sequence 连续至 46。
- Thread Flags Set / Receive 对应至 9。
- Mutex 共享数据更新连续。
- UART 周期输出正常。
- 未观察到 HardFault、Hang 或 Heap 持续下降。

本轮资源：

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

该阶段功能 checkpoint：

`f8c3e52`：`feat: establish M4 ring buffer baseline`

已推送至远程 M4 分支。

重要边界：

- 本轮使用 Thread Flags 串行协调读写阶段。
- 不代表 Ring Buffer 已通过并发安全验证。
- 不代表 Ring Buffer 可以直接在 ISR 中使用。
- 该历史测试本身不代表 ISR → Task 链路；后续 M4.5 已单独建立事件通知链路。
- 该测试阶段原 Semaphore 两个任务被临时复用；已通过后续 `c4c0721` 恢复。
- 在 2026-10-16 的测试固件中原 Semaphore 尚未回归；后续已完成回归。
- 本轮未完成 NRST / Power-cycle 和长期稳定性最终验收。

### M4.3 Semaphore 恢复与可运行基线（2026-10-18）

- 从 Ring Buffer 临时测试中恢复 `SyncSem` 的 Producer Release / Consumer Acquire 正式路径，保留 Ring Buffer 确定性测试代码作为隔离的历史测试入口。
- Consumer `osSemaphoreAcquire(..., osWaitForever)` 可持续阻塞并在 Producer Release 后唤醒；两任务保持循环而不是返回任务入口。
- 板级窗口内 Queue 序号连续、Mutex 更新连续、原 Task Flags 和 UART 正常。
- 恢复阶段 SYS：`SemProFree=288 B`、`SemConFree=288 B`、`Heap=4568 B`、`MinHeap=4568 B`。
- `c4c0721`：`fix: restore M4 semaphore task baseline`；本地与 origin 同步。

### M4.5 TIM7 ISR → Task Thread Flags Baseline（2026-10-20）

新增模块/文件：

```text
Inc/m4_isr.h
Src/m4_isr.c
Inc/tim.h
Src/tim.c
Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_tim.h
```

M4.5 结构：

```text
TIM7 100 ms update interrupt
  -> TIM7_IRQHandler() / HAL_TIM_IRQHandler(&htim7)
  -> main.c 中 HAL_TIM_PeriodElapsedCallback() 的 TIM7 分支
  -> M4ISR_OnTim7ElapsedFromISR()
  -> osThreadFlagsSet(worker_handle, M4_ISR_TIM7_FLAG)
  -> M4IsrWorkerTask / osThreadFlagsWait(osWaitForever)
  -> Task Context 统计与周期日志
```

配置与约束：

- TIM7 Timer Clock 84 MHz、PSC=8399、ARR=999 → 100 ms / 10 Hz。
- TIM6 仍提供 HAL 1 ms Tick，`HAL_IncTick()` 只在 TIM6 分支调用。
- TIM7 NVIC Priority 5；FreeRTOS 中断 API 允许阈值 5；4 bits 抢占优先级；配置已核对。
- ISR 非阻塞、无同步 Logger；Worker 用 Thread Flags 阻塞等待；Worker 创建成功后在 Task context 启动 TIM7。
- `M4ISR_Init()` 等带操作状态的用户接口使用专属枚举；`M4ISR_GetWorkerStackFree()` 为 `uint32_t` 数据查询接口，保持原样。
- 用户应用模块通过根目录 CMake 纳入构建，不把独立业务代码填进 CubeMX 生成的 CMake 源文件清单。

已验证：

```text
irq=30 received=30 errors=0
irq=40 received=40 errors=0
irq=50 received=50 errors=0
irq=60 received=60 errors=0
```

- 同时原 M4Q、M4F、Semaphore、Mutex 和 UART 在已提供日志窗口中正常运行。
- 用户确认后续持续运行测试均通过；未提供完整结束时计数与准确时长，不能补写成新实测数据。
- Worker Stack 配置 1024 B，历史最小剩余 520 B。
- SYS：`Heap=3432 B`、`MinHeap=3432 B`；新增前同口径值为 4568 B，减少 1136 B。
- Clean Build：FLASH 34748 B / 1 MB（3.31%）；静态 RAM 21192 B / 128 KB（16.17%）。
- 最新功能 checkpoint：`adf5d5e`：`feat: establish M4 ISR to task notification baseline`，已推送 origin。

边界：

- 低负载采样点的 `irq == received` 不能证明 Thread Flags 高负载情况下不会合并。
- 这证明的是通知链路，而不是 Ring Buffer SPSC 并发安全或 DMA 数据所有权。
- M4 Final Integration 尚未完成，未证实本阶段存在最终 NRST ×3、Power-cycle ×3 的完整系统级验收。

## 当前已经明确的职责边界

### Queue

适合：

- 结构化数据传递。
- 有限数量的独立消息缓冲。
- 保存每一个成功入队的数据项。
- 建立明确的数据所有权交接边界。

当前 M4 实例：

```text
M4QueueMessage_t
sequence
value
tick
```

### Thread Flags

适合：

- 轻量事件通知。
- 表示某项工作当前处于 Pending 状态。
- Worker 阻塞等待事件。
- 一个线程使用 bit 表达不同事件类型。

不适合直接假设：

- 相同事件的每一次发生都能累计。
- 能够替代结构化 Message Queue。
- 单一 bit 能够作为无损 Event Counter。

### Binary Semaphore

适合：

- Task → Task 同步。
- 表达一个有限状态的“可以继续 / 事件已发生”信号。
- Consumer 在信号不可用时阻塞等待。

当前 M4 实例：

```text
SemProducerTask
→ SyncSem
→ SemConsumerTask
```

不应直接作为：

- 结构化数据 Queue。
- 共享资源所有权锁。
- 无限制事件计数器。

### Mutex

适合：

- 共享资源互斥。
- 建立资源所有权。
- 保护 Read-Modify-Write 临界区。
- 保护共享 buffer / UART 等不可并发访问事务。

当前 M4 实例：

```text
MutexTaskA / MutexTaskB
→ SharedDataMutex
→ shared_value
```

当前 M3 实例：

```text
多个业务 Task
→ Debug Logger Mutex
→ shared log buffer + USART2 transaction
```

已确认：

- 临界区应尽量短。
- Logger 放在 Release 后时，日志输出顺序可能与临界区真实顺序不同。
- 需要用临界区内生成的 sequence 等独立证据判断真实执行顺序。

### Ring Buffer

> 本节为历史基础版本说明；2026-10-27 最新 SPSC 结果以上方快照和本文件末尾的当前验证为准。

> 资料核对：旧记录称 `uint8_t`、物理 8 B / 有效 7 B；此前上传的 `Inc/ring_buffer.h` 快照却为 `uint32_t buffer[8]`。请按最新 HEAD 复核，不能将元素数与字节数混淆。

适合：

- 连续流式数据的有限容量缓存。
- 在生产和消费速率短时不一致时暂存数据。
- 通过 head / tail 维护 FIFO 顺序。
- 通过 Full / Empty 检测边界状态。
- 记录因缓冲区已满而拒绝的写入次数。

当前 M4 实例：

```text
RingBuffer_t

buffer[8]
head
tail
overflow_count
```

当前策略：

- 实际有效容量为 7 B。
- Full 时拒绝写入。
- 不覆盖旧数据。
- Empty 时拒绝读取。
- 不通过历史 overflow_count 决定读取次数。

当前限制：

- 基础功能已经验证。
- 真正的 SPSC 并发访问尚未验证。
- 不允许直接假设 volatile 可以解决全部同步问题。
- ISR 与 Task 的数据所有权尚需明确。
- 覆盖旧数据策略尚未实现。
- 长期生产速率高于消费速率时，Buffer 最终仍可能溢出。

## 当前明确未实现

- M4 Final Integration 的**最终关闭程序**：SPSC OFF 正式固件 Clean Build / 烧录回归、NRST ×3、Power-cycle ×3、合并后最终验收和正式 docs checkpoint。
- SPSC 子分支 → M4 分支的 PR 合并及之后 M4 → main 的 PR 合并，均未收到成功证据。
- ISR ↔ Task Ring Buffer 数据并发、DMA Buffer Ownership、多个生产者/多个消费者访问、覆盖旧数据策略及高负载最坏延迟。
- USART2 正式 RX、M5 ADC + Timer + DMA、M6 UART DMA + IDLE、RS485、Modbus、CAN、Bootloader/IAP、Watchdog / Reset Cause 等后续里程碑。

**已实现且不可再列为未完成：** Queue、Task Flags、Semaphore/Mutex、Ring Buffer Basic、TIM7 ISR→Task 基础通知、Task ↔ Task SPSC 50,000 元素专项。

## 已确认的重要约束

- 硬件外设层采用 STM32 HAL，应用 RTOS 为 CMSIS-RTOS2；TIM6 是 HAL 1 ms Timebase，TIM7 是 M4 ISR 测试 100 ms 中断，SysTick 供 FreeRTOS Tick。
- TIM7 IRQ priority 5、4 bit preemption grouping 与 FreeRTOS Max Syscall priority 5 已按当前配置核查；ISR 不执行同步 Logger / 阻塞 API。
- Ring Buffer 当前 `uint32_t` 元素，8 个物理槽/7 个有效槽；`_Atomic` head/tail 的 acquire/release 规则仅在一个 Producer、一个 Consumer 下成立。
- Full 时拒绝写入并重试是背压模型，`overflow_count` 在本测试中计失败尝试次数，不必然等于丢失业务数据数。
- Thread Flags 相同 bit 的重复 Set 可以合并，当前 10 Hz `irq==received` 仅是采样事实。
- 自定义有状态结果 API 优先返回模块专属枚举；查询数值和无返回值回调可保留原有签名。
- 原 Semaphore 的长期任务职责不可被永久测试代码吞并；SPSC 应使用独立的可选测试模块，正式固件默认关闭。
- 运行资源必须使用目标板 Stack High Water Mark、Free Heap、Minimum Ever Free Heap 观察，不以 ELF 静态 RAM 替代。
- CubeMX 代码再生后检查 `.ioc` 与生成代码一致，Git `text eol=lf` 时定向 LF 规范化并审核 Diff。
- 关闭 M4 前补齐正式固件和本版本复位证据，不提前宣称进入 M5。

## 当前最新验证情况

- 最新专项：2026-10-27 Task ↔ Task SPSC `PASS sent=50000 recv=50000 full=7064 empty=7419`、`overflow=7064 elapsedTicks=7501`。
- Full 和 Empty 均实测覆盖，接收校验未发现跳号、错序、重复；范围限于当前固件/配置的一次可见测试。
- SYS：`SpscPFree=876 B`、`SpscCFree=444 B`、`Heap=MinHeap=3432 B`、`ISRWorkerFree=520 B`；原 Semaphore Stack 288/288 B（本轮 SPSC 版本）。
- 原 M4 在 SPSC 期间同机运行，Queue seq 连续，Semaphore、Thread Flags、Mutex 正常，TIM7 采样点 `irq=70 received=70 errors=0`。
- 另有 Ring Buffer 分阶段自检与原同步模块联合长时间运行的用户确认，通过但没有准确末尾时长；不能混同为无阶段同步 SPSC。
- 新功能 Git `d93665e` 已推送独立分支，用户确认工作区 clean；M4 文档分支最后已知 `405b9ab`，合并尚未确认。
- **未补证据**：本版本 NRST ×3、Power-cycle ×3，OFF 后正式固件构建/烧录回归、PR 合并后的 M4 最终基线与新 docs 提交。

## 下一步最合理的一个任务

**先完成 M4 正式固件退出条件与 SPSC 分支合并，再关闭 M4。**

1. 如尚未完成，在 SPSC 测试 ON 版本补测 NRST ×3 / Power-cycle ×3，并归档 6 次结果；对 PASS 后剩余模块持续运行补充真实终点证据。
2. 显式执行 `cmake --preset Debug -DM4_ENABLE_SPSC_TEST=OFF`、`cmake --build --preset Debug --clean-first`，烧录并确认没有 `[SPSC] START`，其他 M4 功能和 Stack/Heap 依旧正常。
3. 在 GitHub 创建 SPSC → M4 PR（Base=`feature/m4-rtos-communication-baseline`，Compare=`feature/m4-spsc-concurrency-verification`），审核 6 个预期源码文件并完成合并。
4. `git fetch origin`、切换 M4 分支、`git pull --ff-only`，核对合并后的 Git 状态并重新板级回归；把 SPSC / 联合测试记录更新到 `docs/`，建立独立 docs checkpoint。
5. M4 Final Integration 的关闭条件全部满足后，再 PR 合入 `main`，随后创建 M5 开发分支。不能在 PR 成功前将 M4 写成已完成。
