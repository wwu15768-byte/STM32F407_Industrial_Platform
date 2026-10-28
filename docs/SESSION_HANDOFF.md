> 当前会话快照；会话切换前整体更新。
> 更新时间：2026-10-28（M5-P0 板级验收后，Git 功能/文档归档前）。
> 最新用户确认 Git 基线：`feature/m5-adc-timer-dma-baseline`，清理前 HEAD / main / origin/main 均为 `e793039`（M4 PR #5 Merge）；此前 M4 已关闭并合并。
> 以下“已完成并验证的内容”中 M0～M4 的 Queue / Semaphore / ISR 等任务结构只描述**当时的历史实验版本**，不代表当前固件仍在创建这些任务；最新运行结构以本页开头及末尾为准。

## 当前 Milestone

M5：ADC + Timer + DMA 实时数据采集子系统；当前 M5-P0（清理历史演示任务和日志）**Debug 增量构建、30 秒 UART、NRST ×3、Power-cycle ×3 已通过**，剩余文档导入与 Git Commit/Push。M0～M4 已完成。

- M4 SPSC 并发验证：`d93665e`，经 PR #4 Merge `ccbd176` 合入 M4；M4 关闭文档 `3a8fa13`；PR #5 Merge `e793039` 合入 main。
- M5-P0 目标运行态：仅 `HeartBeatTask` 和 `UARTTask` 两个应用任务；M4 Queue/Flags/Semaphore/Mutex/ISR 演示任务不再默认创建。Ring Buffer、Debug Logger 保留；`m4_ipc` / `m4_isr` 保留精简接口 / 声明，默认初始化不再调用原实验启动函数。
- M5-P1 及之后的 ADC/Timer/DMA 采集功能尚未实现，不把 SPSC Task ↔ Task 测试结果直接视为 DMA 数据所有权验证。

## 当前 Branch 与 Git 基线

- Branch：`feature/m5-adc-timer-dma-baseline`。
- M5-P0 清理前起点：`e793039`（同时对应 main、origin/main 和此分支当时的 HEAD）。
- 清理前标签：`m5-pre-cleanup-20261028`，此前上传工程归档中检查到；本次未再次核对远程 Tag。
- 当前代码已经编辑、构建及烧录验证；本次改动和 docs ZIP 尚未得到新的 Commit Hash、Push / clean working tree 的用户确认，不得将 `e793039` 误写为本次清理提交。

## 2026-10-28 M5-P0 最新验证快照

- `.ioc` 的 6 个应用任务减少为 2 个，移除 `SyncSem` 和 `SharedDataMutex`；`freertos.c` 删除演示 Task/变量，并去掉 `M4Ipc_Init()`、`M4ISR_Init()` 的默认调用；保留条件 SPSC 测试入口。
- Debug 命令 `cmake --build --preset Debug` 返回成功，输出 `[2/2] Linking`（增量构建）；RAM `21128 B (16.12%)`、Flash `27916 B (2.66%)`、CCMRAM `0`。未提供本轮独立 Clean Build 证据。
- 清理前 30 秒日志 `882` 行、Heap `3432 B`；清理后 30 秒 `9` 行、Heap/MinHeap `13504 B`；差额 `+10072 B`，减少约 `98.98%` 日志。
- 清理后 HeartFree `384 B`；UARTFree 初期 `872 B`、后续最低可见 `520 B`。用户实际确认应用 Task 数量为 `2`；30 秒与复位日志未见 M4 演示输出。
- 用户完成 NRST ×3 和 Power-cycle ×3，均报告输出正常；复位日志偶有 `€` 前缀，后续输出完整，尚不能据此确认为固件 Bug。
- Git 格式整理：57 个修改文件中的无业务差异 FreeRTOS 文件已恢复；最新所见 11 个修改文件中 7 个有实质内容差异，`git -c core.safecrlf=false diff --check` 无空白错误。最终是否已只剩 7 个文件、是否提交仍待用户提供。

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

- M5 ADC Regular Conversion、Timer TRGO 外部触发、DMA Circular Half/Full、FreeRTOS Acquisition/Processing Task、数据所有权和过载检测，均尚未开发。
- ISR ↔ Task Ring Buffer 真实并发、DMA Buffer Ownership、MPSC/MPMC、最坏采样延迟、长期高负载可靠性，仍为未验证边界。
- USART2 正式 RX、M6 UART DMA + IDLE、RS485、Modbus、CAN、Bootloader/IAP、Watchdog/Reset Cause 等后续里程碑。
- 本轮尚未收到 `cmake --build --preset Debug --clean-first`、M5-P0 代码 Commit/Push 与最新 clean working tree 证据；SPSC ON 独立回归不属于本次双任务正式运行验收。

**已实现且不可再列为未完成：** M4 Queue、Task Flags、Semaphore/Mutex、Ring Buffer Basic、TIM7 ISR→Task 基础通知和 Task ↔ Task SPSC 50,000 元素专项的历史验证；M4 PR 合并与关闭；M5-P0 双任务运行态及 6 次复位的板级验证。

## 已确认的重要约束

- MCU STM32F407VGT6、HAL + CMSIS-RTOS2；TIM6 是 HAL 1 ms Timebase，SysTick 用于 FreeRTOS Tick。TIM7 曾用于 M4 ISR 10 Hz 实验，目前仍有生成配置，默认程序不再启动该演示链路；M5 选定采样定时器之前先核查资源。
- `debug_log` 使用任务上下文中的同步 UART 与 Mutex，非 ISR-safe；采集 ISR 中不能直接打印或执行阻塞操作。
- SPSC Ring Buffer 是 8 个 `uint32_t` 物理槽 / 7 个有效槽；`_Atomic` head/tail 的 acquire/release 只覆盖一个 Task Producer 与一个 Task Consumer，不能自动推广到 DMA 写入的缓冲区。
- Thread Flags 重复 Set 相同 bit 可以合并；DMA Half/Full 到任务的通知必须设计有界处理和 Buffer Ownership，不能单靠 Flag 假定样本无丢失。
- 自定义模块函数遵循专属状态码等已接受规范；保留接口应有清晰调用场景，不在正式运行态凭保留接口自动分配无用对象。
- CubeMX Generate Code 后复核 `.ioc`、`freertos.c`、TIM6/TIM7、CMake 和 Git EOL Diff；不要无审查地覆盖用户代码或暂存第三方文件。
- Stack / Heap 以实际板级采样为准；ELF 静态 RAM 占用不等于 FreeRTOS 可用 Heap。

## 当前最新验证情况

- M5-P0 Debug：增量构建 PASS；RAM 21128 B、FLASH 27916 B。
- M5-P0 UART / 资源：30 秒 `9` 行，`HeartFree=384 B`、`UARTFree` 最低可见 `520 B`、`Heap=MinHeap=13504 B`；旧 M4 版本对照为 882 行和 Heap 3432 B。
- 应用任务数：用户确认 2；NRST ×3 PASS，Power-cycle ×3 PASS（用户实测报告）。
- M4 历史 SPSC：`PASS sent=50000 recv=50000 full=7064 empty=7419`，`overflow=7064 elapsedTicks=7501`，Git/PR 历史 `d93665e` → `ccbd176` → `3a8fa13` → `e793039`。
- Git 尚未收到本次 M5-P0 新提交与推送证据；docs ZIP 不能自动代替 Git 提交。

## 下一步最合理的一个任务

**先完成 M5-P0 功能和文档的 Git 归档，再启动 ADC 资源配置。**

1. 将此 ZIP 的九份 Markdown 导入原仓库 `docs/`，审查文档与代码 Diff；如果有剩余 EOL-only 文件，确认无需要保留的格式编辑后再定向恢复。
2. 执行 `git status --short`、`git diff --ignore-space-at-eol --stat`、`git -c core.safecrlf=false diff --check`；如需独立 Clean Build 证据，再运行 `cmake --build --preset Debug --clean-first` 并记录结果。
3. 精确选择 M5-P0 功能源文件及本次 docs 文件暂存；执行 `git diff --cached --check`、`git diff --cached --stat`，随后 Commit / Push 到 `feature/m5-adc-timer-dma-baseline`，确认状态 clean。
4. M5-P1 核对 ADC 接线和引脚、定时器 TRGO、DMA Stream/Channel、采样速率与 DMA 半缓冲区所有权；形成资源冲突表和实验验收指标，之后才做 CubeMX 新配置与代码生成。
