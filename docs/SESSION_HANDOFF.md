> 当前会话快照；会话切换前整体更新。


> 当前会话快照；会话切换前整体更新。
> 更新时间：2026-10-16。

## 当前 Milestone

M4：RTOS 通信、同步和数据缓冲，正在进行。

M0、M1、M2、M3 均已完成。

M4.1 Queue Producer / Consumer baseline 已完成。

M4.2 Task Notification / CMSIS-RTOS2 Thread Flags baseline 已完成。

M4.3 Binary Semaphore Task → Task 同步和 Mutex 共享资源保护对比已完成历史板级验证、联合运行和 Stack / Heap 调优。

M4.4 Ring Buffer 基础功能已完成真实板级确定性测试。

Ring Buffer 已验证：

- Empty。
- Full。
- Full 后数据保留。
- Write Index Wrap-around。
- Read Index Wrap-around。
- FIFO 数据顺序。
- Full 写入失败和溢出统计。

当前下一步为 M4.5 ISR → Task 通信边界。

M4 整体尚未关闭。

暂不进入 M5 ADC + Timer + DMA。

## 当前 Branch 与 Git 基线

- 当前 Branch：`feature/m4-rtos-communication-baseline`。
- Upstream：`origin/feature/m4-rtos-communication-baseline`。
- 最新 Ring Buffer 功能 checkpoint：`f8c3e52`，`feat: establish M4 ring buffer baseline`。
- 最新 M4 同步文档 checkpoint：`5c9efac`，`docs: record M4 synchronization baseline verification`。
- 同步任务 Stack 调优 checkpoint：`c84fed8`。
- Mutex checkpoint：`c5bc54a`。
- Binary Semaphore checkpoint：`435ba4f`。
- CMake 修复 checkpoint：`34ddce3`。
- Thread Flags 文档 checkpoint：`1008734`。
- Thread Flags 功能 checkpoint：`fd80cc9`。
- Queue 文档 checkpoint：`22d25bd`。
- Queue 功能 checkpoint：`b78282c`。
- M4 起点：`c13b8e5`。
- `f8c3e52` 已成功推送至远程。
- 功能提交完成后已确认 Working Tree clean。
- 本次 docs 文档同步尚未形成新的 Git checkpoint。

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

### M4.4 Ring Buffer Baseline

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

本轮临时复用：

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

当前功能 checkpoint：

`f8c3e52`：`feat: establish M4 ring buffer baseline`

已推送至远程 M4 分支。

重要边界：

- 本轮使用 Thread Flags 串行协调读写阶段。
- 不代表 Ring Buffer 已通过并发安全验证。
- 不代表 Ring Buffer 可以直接在 ISR 中使用。
- 不代表 ISR → Task 链路已经建立。
- 本轮原 Semaphore 两个任务被临时复用。
- 原 Binary Semaphore 功能尚未在当前测试固件中重新回归。
- 本轮未完成 NRST / Power-cycle 和长期稳定性最终验收。

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

- ISR → Task Thread Flags / Queue / Semaphore 实机通信。
- ISR-safe 通信边界的完整验证。
- Ring Buffer SPSC 并发安全。
- Ring Buffer 覆盖旧数据模式。
- 多 Flag bit 组合实验。
- Mutex Priority Inheritance / Priority Inversion 专门实验。
- USART2 RX 正式通信链路。
- ADC + Timer + DMA。
- UART DMA + IDLE。
- RS485。
- Modbus。
- 统一数据模型。
- CAN。
- Bootloader / IAP。
- Watchdog。
- Reset Cause。
- Fault Diagnostics。

Ring Buffer 基础数据结构与 Empty / Full / Wrap-around 不再属于未完成项。

## 已确认的重要约束

- STM32 外设层继续使用 HAL。
- FreeRTOS 应用层继续使用 CMSIS-RTOS2。
- M4 IPC 逻辑应继续保持独立应用模块化。
- Ring Buffer 核心模块不依赖 HAL / FreeRTOS。
- `freertos.c` 不应长期堆积业务实验代码。
- Queue 用于结构化数据和有限消息缓冲。
- Thread Flags 用于轻量事件状态。
- Binary Semaphore 用于同步。
- Mutex 用于共享资源互斥。
- Ring Buffer 用于连续流式数据缓存。
- Thread Flags 不作为无损事件计数器。
- Ring Buffer 不解决长期生产速度高于消费速度的问题。
- ISR 应保持最小执行路径。
- ISR 不使用当前同步 `LOG_xxx()`。
- ISR 内不调用阻塞式 API。
- 对 ISR-safe API 必须核对 CMSIS-RTOS2 规范。
- ISR 与 Task 共享数据之前必须明确访问所有权和同步要求。
- 当前基础 Ring Buffer 测试通过不代表并发安全通过。
- 每新增重要 Task 或 RTOS Object 后重新测量 Stack 和 FreeRTOS Heap。
- 临时压力与测试代码必须在 Final Baseline 前恢复或隔离。
- M4 正式关闭前不进入 M5。

## 当前最新验证情况

当前最新为 M4 Ring Buffer 确定性板级功能测试。

已验证：

- Ring Buffer Empty。
- Ring Buffer Full。
- Full 后已有数据保持。
- Write Index Wrap-around。
- Read Index Wrap-around。
- FIFO 数据顺序。
- Empty 读取失败时输出变量不变。
- Full 拒绝写入和 overflow_count。
- Queue、Thread Flags、Mutex、UART 当前窗口继续运行。
- FreeRTOS Heap=4568 B。
- FreeRTOS MinHeap=4568 B。

尚未重新验证：

- 当前测试固件中的原 Binary Semaphore 同步链路。
- Ring Buffer ISR / Task 并发安全。
- ISR → Task 通信链路。
- M4 最终长期稳定性。
- M4 最终 NRST ×3 和 Power-cycle ×3。

当前需要特别注意：

- SemProducerTask / SemConsumerTask 已被临时复用于 Ring Buffer。
- 不应直接将当前固件视为完整 M4 Final Baseline。
- 后续修改任务代码前应保留已经验证的 Ring Buffer 测试证据。
- 原 Semaphore 基础功能历史 checkpoint 仍然有效。

当前功能 checkpoint：

`f8c3e52`：`feat: establish M4 ring buffer baseline`

Remote：

`origin/feature/m4-rtos-communication-baseline`

## 下一步最合理的一个任务

进入 M4.5 ISR → Task 通信边界。

先完成必要的测试环境整理：

1. 恢复或隔离 Ring Buffer 临时测试任务。
2. 保留 Ring Buffer 确定性测试代码及测试记录。
3. 恢复原 Binary Semaphore 实验的可运行状态。
4. 检查当前代码与 `DEC-20260930-01` 模块化决策的一致性。

然后建立最小 ISR → Task 通信实验：

1. 选择一个可控的真实中断事件源。
2. 核对 CMSIS-RTOS2 在 ISR 中允许调用的 API。
3. ISR 内只执行必要的事件通知或状态记录。
4. 不在 ISR 中调用当前同步 LOG_xxx()。
5. Consumer Task 使用阻塞等待，不采用 Busy Polling。
6. 验证中断产生、Task 被唤醒、Task 处理事件的链路。
7. 验证重复事件和事件状态边界。
8. 再研究 Ring Buffer SPSC 数据所有权。
9. 检查共享索引的原子性、内存可见性和必要同步。
10. 重新记录 Stack / Heap 和异常路径资源。

M4.5 通过后再进行 M4 Final Integration：

- 恢复全部需要保留的正式功能。
- 完成 Queue、Thread Flags、Semaphore、Mutex、Ring Buffer 联合回归。
- 完成 ISR → Task 最终验证。
- 检查模块边界与 CubeMX 生成代码隔离。
- 重新完成构建资源统计。
- 完成长期运行及复位恢复。
- 更新项目文档和 Git checkpoint。
- 再决定 M4 是否正式关闭。

暂不进入 M5 ADC + Timer + DMA。
