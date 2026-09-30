# SESSION_HANDOFF.md

> 当前会话快照；会话切换前整体更新。

## 当前 Milestone

M4：RTOS 通信、同步和数据缓冲，正在进行。

M0、M1、M2、M3 均已完成。

M4.1 Queue Producer / Consumer baseline 已完成并形成独立 Git checkpoint。当前下一步为 M4.2 Task Notification / CMSIS-RTOS2 Thread Flags。

## 当前 Branch 与 Git 基线

- 当前 Branch：`feature/m4-rtos-communication-baseline`。
- Upstream：`origin/feature/m4-rtos-communication-baseline`。
- 当前 HEAD：`b78282c`，`feat: establish RTOS queue communication baseline`。
- M4 路线更新 checkpoint：`ed6c3a7`，`docs: update roadmap for expanded project milestones`。
- M4 起点：`c13b8e5`，M3 Merge Commit。
- 当前 `main` / `origin/main`：`c13b8e5`，已经包含完整 M0～M3。
- 当前 M4 分支已经推送到远程并与 upstream 同步。
- Working Tree：clean。

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

- 已建立独立：
    - `Inc/m4_ipc.h`
    - `Src/m4_ipc.c`
- M4 用户模块通过根目录 `CMakeLists.txt` 加入构建。
- `freertos.c` 只负责调用 `M4Ipc_Init()`，具体 M4 IPC 对象和任务逻辑保持在 `m4_ipc.c`。
- 已建立：

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

- `M4QueueMessage_t` 当前字段：
    - `uint32_t sequence`
    - `uint32_t value`
    - `uint32_t tick`
- Queue Item Size：12 B。
- Queue Length：8。
- Producer Priority：`osPriorityNormal`。
- Producer Stack：1024 B。
- Consumer Priority：`osPriorityNormal`。
- Consumer Stack：1024 B。
- Final Producer Period：200 ms。
- Queue Put Timeout：0。
- Consumer Queue Get Timeout：`osWaitForever`。
- Queue 采用结构体按值复制，不传递 Producer 局部对象指针。

## M4.1 已验证行为

### 正常路径

实际观察到连续输出：

```text
[I][M4Q] rx seq=1 value=10 tick=1 count=0
[I][M4Q] rx seq=2 value=20 tick=201 count=0
[I][M4Q] rx seq=3 value=30 tick=401 count=0
...
```

已确认：

- Sequence 连续。
- `value = sequence × 10`。
- Tick 基本每 200 tick 增加一次。
- Consumer 收到消息后 Queue Count 正常回到 0。
- 未出现 Queue Full。
- 未出现 Sequence Gap。
- HeartBeatTask 和 UARTTask 继续正常运行。

### Queue Full / Backpressure 压力实验

临时测试配置：

- Producer Period：约 50 ms。
- Consumer 人工 Delay：约 500 ms。
- Queue Length：8。
- Put Timeout：0。

实际观察到：

```text
[W][M4Q] queue full: drop=1 seq=10 count=8
...
[W][M4Q] sequence gap: expected=10 actual=12
...
```

已确认：

- Producer 长期快于 Consumer 时 Queue Occupancy 会持续增加。
- Queue 最终达到 Full。
- timeout = 0 时 Put 不阻塞，立即返回失败。
- Drop Counter 能够记录发送失败。
- Queue Full 不会破坏此前已经成功进入 Queue 的旧消息。
- Consumer 会继续消费 Queue 内原有连续数据。
- 当旧库存逐步消费完成后，Consumer 能够检测到 Sequence Gap。
- Queue 只能吸收短时间突发，不能解决长期吞吐量失衡。

## M4.1 资源验证

初始 Producer Stack = 512 B 时，正常路径资源数据曾观察到：

```text
HeartFree=384 B
UARTFree=496 B
M4ProdFree=296 B
M4ConsFree=496 B
Heap=11560 B
MinHeap=11560 B
```

在 Queue Full + `LOG_WARN()` 压力路径下：

```text
M4ProdFree=0 B
```

因此确认初始 512 B Producer Stack 对异常日志路径缺少安全余量。

将 Producer Stack 调整为 1024 B 后，在相同压力条件下稳定观察到：

```text
HeartFree=384 B
UARTFree=496 B
M4ProdFree=512 B
M4ConsFree=496 B
Heap=11048 B
MinHeap=11048 B
```

结论：

- Producer Stack 最终保留 1024 B。
- Consumer Stack 保留 1024 B。
- Producer 增加 512 B Stack 后 FreeRTOS Free Heap 同样减少 512 B。
- 当前 Heap 与 Stack 水位稳定。
- Stack 必须按最坏执行路径而不是只按正常路径评估。

## M4.1 Final Baseline

压力实验完成后已经恢复：

- Producer Period：200 ms。
- 删除 Consumer 人工 500 ms Delay。
- Queue Length：8。
- Producer Stack：1024 B。
- Consumer Stack：1024 B。
- Put Timeout：0。
- Consumer：`osWaitForever`。

恢复后的 Final Baseline 稳定性测试已经通过。

未观察到：

- Queue Full。
- Sequence Gap。
- Heap 持续下降。
- Hang。
- HardFault。

M4.1 最终功能 checkpoint：

`b78282c`：`feat: establish RTOS queue communication baseline`

## 当前明确未实现

- Task Notification / CMSIS-RTOS2 Thread Flags 尚未实现。
- Binary Semaphore 尚未实现。
- Mutex 与 Binary Semaphore 的系统对比尚未完成。
- Ring Buffer 尚未实现。
- ISR → Task 通知边界尚未验证。
- 当前 `LOG_xxx()` 不是 ISR-safe API。
- USART2 RX 尚未进入正式通信实现。
- ADC + Timer + DMA 属于 M5，尚未开始。
- UART DMA + IDLE、RS485、Modbus、统一数据模型、CAN、Bootloader、Watchdog、Reset Cause 和 Fault Diagnostics 均属于后续里程碑。

## 已确认的重要约束

- STM32 外设层继续使用 HAL。
- FreeRTOS 应用层继续使用 CMSIS-RTOS2。
- M4 应用层通信逻辑保持模块化，不继续把具体实验逻辑堆入 `freertos.c`。
- CubeMX 生成文件不作为用户模块源码注册的主要修改位置；用户源码优先通过根目录 `CMakeLists.txt` 管理。
- 第一版 Queue 使用小结构体按值传递，不引入指针生命周期问题。
- ISR 保持最小，不调用当前同步日志接口，不执行复杂计算或阻塞 API。
- 每新增重要 Task 或 RTOS Object 后重新测量 Stack 和 FreeRTOS Heap。
- 压力测试临时参数必须在形成 Final Baseline 前恢复。
- M4 完成前不进入 M5。

## 最近一次有效功能测试

M4.1 已完成：

- Queue Producer / Consumer 正常路径验证。
- 结构体按值传递验证。
- Consumer Blocking Receive 验证。
- Queue Full / Backpressure 压力验证。
- Drop Counter 验证。
- Sequence Gap 检测。
- Producer Stack 压力路径验证。
- Producer Stack 512 B → 1024 B 修正及重新压力验证。
- Final Baseline 稳定性验证。

当前最新功能 checkpoint：

`b78282c`：`feat: establish RTOS queue communication baseline`

## 下一步最合理的一个任务

进入 M4.2 Task Notification / CMSIS-RTOS2 Thread Flags。

建立最小 Task → Task 通知实验：

1. 定义一个明确的 Event Flag。
2. 一个 Task 周期产生事件。
3. 一个 Worker Task 使用阻塞等待接收事件。
4. 验证没有事件时 Worker 处于 Blocked。
5. 验证事件到达后 Worker 被唤醒。
6. 验证连续或重复事件的基本行为。
7. 明确比较：
   - Queue：用于携带结构化数据。
   - Task Notification / Thread Flags：用于轻量事件通知。
8. 重新测量新增 Task / Notification 后 Stack 与 Heap。
9. 验证完成后建立独立 Git checkpoint。

暂不进入 Binary Semaphore、Ring Buffer 或 M5 ADC + Timer + DMA，直到 Notification 实验已经完成并能够解释。