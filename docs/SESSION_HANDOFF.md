> 当前会话快照；会话切换前整体更新。
> 更新时间：2026-10-29（M5 ADC 采集链及用户报告的长时间/复位回归通过，当前功能与文档 Git 归档前）。
> 最近一次有终端证据的 Git clean HEAD：`feature/m5-adc-timer-dma-baseline` @ `e7f525c`（M5-P0 已推送）；M5 新功能尚未收到新的 Git HEAD/Push 输出。
> 以下长篇 M0～M4 历史实验段落中写“当前结构”的内容，仅代表**该实验当时的代码结构**，不代表今天默认固件中仍创建对应演示任务。实时状态以本段和文件末尾为准。

## 当前 Milestone

M5：ADC + Timer + DMA 实时数据采集；**P0 已功能/文档归档，P1～P5 基础功能、延时测试与用户报告的长时间及复位回归均完成，剩余本轮 ADC 功能/文档提交**。M0～M4 已完成。

- M5-P0：从历史 M4 多任务演示清理到仅 `HeartBeatTask` / `UARTTask` 双任务；功能 `942fcc3`、文档 `e7f525c`，已推送。
- M5-P1：PA1/ADC1_IN1 + 10 kΩ 电位器；TIM2 Update TRGO 50 Hz（设计）、DMA2 Stream0 Circular，替代 M4 期间 TIM7 ISR 实验路径。
- M5-P2/P4：新增第三个应用任务 `AcquisitionTask`（），完成 DMA Half/Full 回调通知 → 10 点平均 → mV 换算 → UART 摘要；板级电压范围 2～3287 mV。
- M5-P3：0/50/250 Tick 人为延时对比与 Late 计数完成，250 Tick 触发 Late 27→57→87，正式代码已恢复无人工延时。
- M5-P5：用户确认长时间运行正常、NRST ×3 和 Power-cycle ×3 均正常，未记录准确时长。

## 当前 Branch 与 Git 基线

- Branch：`feature/m5-adc-timer-dma-baseline`，最近一次用户确认与 `origin/feature/m5-adc-timer-dma-baseline` 同步且工作区 clean 的 HEAD=`e7f525c`。
- M5-P0 功能提交=`942fcc3`；文档提交=`e7f525c`；清理前标签=`m5-pre-cleanup-20261028`，`main` / `origin/main` 最近确认仍为 `e793039`。
- 当前 ADC/TIM2/DMA 版本已构建/烧录/实机回归，但最新代码 Diff、暂存区、Commit/Push 尚待确认；不要虚构 ADC 功能提交 Hash，也不要宣称新功能已合入 main。

## 2026-10-29 M5-P1～P5 最新验证快照

- TIM2 时钟 84 MHz、PSC=8399、ARR=199、TRGO Update；ADC1_IN1（PA1）12 bit、21 MHz 时钟、84 cycles 采样时间、TIM2 TRGO Rising 触发；DMA2 Stream0 Circular、20×`uint16_t`（每半区 10 个），默认无 TIM2 ISR 每采样调用。
- `AcquisitionTask` 启动 ADC DMA 再启动 TIM2；Half/Full Callback 更新计数、设置 Thread Flags 并记录 Tick，任务读取对应半区计算平均毫伏值，UART 约每 10 秒记录摘要。
- 电位器从最低逐步旋至最高测得 `2, 612, 1065, 1451, 2250, 2974, 3287, 3285, 3286 mV`；没有万用表绝对精度对照。
- 正常采集计数：`Half=23/23→48/48→73/73`，`Full=22/22→47/47→72/72`（Processed/Callback）；`Late=0 Error=0`，同一种 Callback 每约 10 秒增加 25 次，属于设计级频率核对。
- A 0 Tick：最后 Half 73/73、Full 72/72、Late 0；B 50 Tick：Half 72/73、Full 72/72、Late 0；C 250 Tick：Half 72/73、Full 71/72、Late 27→57→87。C 组确认超时风险，不证明具体样本覆盖数量。
- 恢复无延时后重新 Debug 增量构建 PASS，RAM `21448 B (16.36%)`、Flash `35680 B (3.40%)`、CCMRAM `0`；Heap/MinHeap `12368 B`、HeartFree `384 B`、UARTFree 最低可见 `512 B`。
- 用户确认长时间运行 PASS、NRST ×3 PASS、完全断电 Power-cycle ×3 PASS；未给出运行精确分钟数或长期全量 UART 记录。

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

- 严格逐样本/逐半块无丢失证明、DMA Buffer 的完整所有权及覆盖检测、Thread Flags 合并后的精确丢块统计、可量化最坏任务响应时间。
- ADC 绝对电压精度标定、外部仪器精密采样频率测量、工业传感器信号调理及复杂多通道采样。
- M5 原路线图中可扩展的独立 Queue → Processing Task 架构尚未引入；本阶段在 `AcquisitionTask` 内完成平均值处理，符合最小功能目标。
- USART2 正式 RX、M6 UART DMA + IDLE、RS485、Modbus、CAN、Bootloader/IAP、Watchdog/Reset Cause 等后续里程碑。
- `cmake --build --preset Debug --clean-first` 的独立 Clean Build 和本轮 ADC 新功能 Commit/Push 尚无用户提供的输出。

**已实现且不可再列为未完成：** M4 Queue/Task Flags/Semaphore/Mutex/Ring Buffer/ISR 历史实验、M4 Task↔Task SPSC 50,000 专项、M4 PR 合并、M5-P0 清理并推送、M5 ADC/TIM2/DMA/AcquisitionTask 基础采样与延时/复位回归。

## 已确认的重要约束

- MCU STM32F407VGT6、HAL + CMSIS-RTOS2；TIM6 是 HAL Timebase、SysTick 为 FreeRTOS Tick；本次 ADC 规则转换由 TIM2 TRGO 硬件触发，旧 TIM7 实验模块不在正式采集主链路中。
- 当前信号接线为 10 kΩ 电位器滑动端到 PA1/ADC1_IN1，固定端到 3.3 V/GND；不可将 5 V/12 V 直接输入 ADC。
- `debug_log` 仍是 Task context 的同步 UART + Mutex，非 ISR-safe；ADC/DMA 回调只做轻量处理，不逐样本输出串口。
- DMA2 Stream0 Circular buffer 20 个 `uint16_t`、前后各 10 个；DMA 可写 SRAM 不等于 CPU 保证能在覆盖前读取。不要将 buffer 放在 CCMRAM。
- 50 Hz 下两个 Half/Full 事件交替约 200 ms，同一种事件约每 400 ms；Thread Flags 按 bit 合并，计数相等、Late=0、Error=0 仅能支持已有窗口的初步结论。Late 使用最近一次 Callback Tick，在重复事件时有漏报可能。
- 人为延时压力代码应保持关闭，正式回归版本已恢复无额外 `osDelay(250U)`；必要时才重新开启有意的专项实验。
- CMake / CubeMX 再生成后要检查 `.ioc`、ADC、DMA、TIM2、`freertos.c`、生成的 CMake 及 CRLF/LF 差异；不直接修改 FreeRTOS 内核或无审查批量提交第三方文件。
- `M4_ENABLE_SPSC_TEST` 默认 OFF，实验 ON 配置只在需回归 M4 SPSC 时使用。

## 当前最新验证情况

- M5-P0 Git：`942fcc3` 功能与 `e7f525c` 文档已推送；最后一次确认工作区 clean、分支与远程同步。
- M5-P1/P2/P4：10 kΩ 电位器 2～3287 mV 全量程变化；TIM2 TRGO / ADC1_IN1 / DMA2 Circular / CMSIS Thread Flags / `AcquisitionTask` 完整主链路实测。
- M5-P3：0 / 50 Tick `Late=0`，250 Tick `Late=27→57→87`；恢复正常后 `Half=73/73 Full=72/72 Late=0 Error=0`。
- M5-P5：用户报告长时间运行通过；NRST ×3、Power-cycle ×3 均通过；未提供精确时长、最坏负载时序或逐样本无丢失证据。
- 最新 Debug 增量构建：RAM 21448 B / 16.36%，Flash 35680 B / 3.40%，Heap/MinHeap 12368 B；`--clean-first` 尚未有独立证据。
- 证据记录：`TEST-20261028-04/05`、`TEST-20261029-01/02/03`，相关设计 `DEC-20261028-03`、`DEC-20261029-01/02`，知识边界 `KG-014`。

## 下一步最合理的一个任务

**完成 M5 本轮 ADC/TIM2/DMA 功能代码与九份项目文档的 Git 归档，不再重复增加基础采样代码。**

1. 导入本次 docs ZIP，确认是否有覆盖之后仍需保留的本地新改动；审查源码、`.ioc`、CMake 的真实差异和历史 EOL 规范化风险。
2. 检查 `git status --short`、`git diff --ignore-space-at-eol --stat`、`git diff --check`；可选补做 Debug `--clean-first` 并如实记录。
3. 按功能文件与 docs 文档分别暂存、`git diff --cached --check`、`git diff --cached --stat`，提交并推送 `feature/m5-adc-timer-dma-baseline`；确认新 HEAD、远程同步与 clean 工作区后再填写新 Commit Hash。
4. 若无新错误，后续进入下一里程碑准备；严格逐块无损与精度标定留为已知技术边界，避免把暂未验证的能力写成完成。
