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
