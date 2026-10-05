> 当前会话快照；会话切换前整体更新。

## 当前 Milestone

M4：RTOS 通信、同步和数据缓冲，正在进行。

M0、M1、M2、M3 均已完成。

M4.1 Queue Producer / Consumer baseline 已完成。

M4.2 Task Notification / CMSIS-RTOS2 Thread Flags baseline 已完成真实板级验证。

当前下一步为 M4.3 Binary Semaphore。

## 当前 Branch 与 Git 基线

- 当前 Branch：`feature/m4-rtos-communication-baseline`。
- Upstream：`origin/feature/m4-rtos-communication-baseline`。
- M4.2 最新功能 checkpoint：`<fd80cc9>`，`feat: establish RTOS thread flag notification baseline`。
- M4.1 文档 checkpoint：`22d25bd`，`docs: record M4 queue baseline verification`。
- M4.1 Queue 功能 checkpoint：`b78282c`，`feat: establish RTOS queue communication baseline`。
- M4 路线更新 checkpoint：`ed6c3a7`，`docs: update roadmap for expanded project milestones`。
- M4 起点：`c13b8e5`，M3 Merge Commit。
- 当前 `main` / `origin/main`：`c13b8e5`，已经包含完整 M0～M3。

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

## M4.2 资源验证

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

## M4.2 Final Baseline

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

### Mutex

当前已经在 M3 Debug Logger 使用。

主要用于：

- 保护共享资源所有权。
- 防止多个 Task 同时操作共享日志 buffer 和 UART 发送事务。

Binary Semaphore 尚未实机验证，下一步专门与 Mutex 对比。

## 当前明确未实现

- Binary Semaphore。
- Binary Semaphore 与 Mutex 的系统对比。
- Ring Buffer。
- 多 Flag bit 组合实验。
- ISR → Task Thread Flags / Queue。
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

## 已确认的重要约束

- STM32 外设层继续使用 HAL。
- FreeRTOS 应用层继续使用 CMSIS-RTOS2。
- M4 IPC 逻辑继续保持在独立应用模块中。
- 不继续将具体实验逻辑堆入 `freertos.c`。
- Queue 用于结构化数据，Thread Flags 用于轻量事件状态。
- Thread Flags 不作为无损事件计数器。
- ISR 保持最小。
- 当前 `LOG_xxx()` 不是 ISR-safe API。
- 每新增重要 Task 或 RTOS Object 后重新测量 Stack 和 FreeRTOS Heap。
- 临时压力测试参数必须在形成 Final Baseline 前恢复。
- M4 完成前不进入 M5 实现。

## 最近一次有效功能测试

M4.2 已完成：

- Task → Task Thread Flags 正常通知。
- Worker Blocking Wait。
- Event Set 后 Worker 唤醒。
- 正常路径 11 Sets → 11 Receives。
- Thread Flags 与 Queue 并行运行。
- 相同 Flag bit 三连 Set Burst。
- 33 Sets → 11 Receives 合并语义验证。
- Burst 压力 Stack 测量。
- Final Baseline Stack 和 Heap 测量。
- NRST ×3。
- Power-cycle ×3。

当前最新功能 checkpoint：

`<fd80cc9>`：`feat: establish RTOS thread flag notification baseline`

## 下一步最合理的一个任务

进入 M4.3 Binary Semaphore。

先建立最小 Task → Task 同步链路：

1. 一个 Task 周期 Give Binary Semaphore。
2. 一个 Worker Task 使用阻塞 Take。
3. 验证 Semaphore 不可用时 Worker 进入 Blocked。
4. 验证 Give 后 Worker 被唤醒。
5. 验证重复 Give 的 Binary 上限行为。
6. 对比：
   - Queue：数据传递。
   - Thread Flags：事件状态。
   - Binary Semaphore：同步信号。
   - Mutex：共享资源所有权保护。
7. 重新测量 Stack 和 FreeRTOS Heap。
8. 完成后建立独立 Git checkpoint。

Binary Semaphore 验证完成后再进入 Ring Buffer 和 ISR → Task 边界。

暂不进入 M5 ADC + Timer + DMA。
