# 错误与故障档案

> 唯一回答：什么坏了，为什么，怎么修？
> 更新时机：出现耗时、典型、易复发或暴露知识缺口的问题时。

## ERR-20260908-01：目标 MCU 型号与实际开发板不一致

### 日期

2026-09-08

### 模块

项目配置与硬件基线。

### 当前环境

M1 CubeMX 工程生成前，仓库尚无固件代码。

### 问题现象

README 将目标 MCU 记录为 STM32F407ZGT6，而实际开发板使用 STM32F407VGT6。

### 期望行为

项目文档和后续 CubeMX 工程都应使用实物上的 STM32F407VGT6、LQFP-100。

### 实际行为

原记录指向 STM32F407ZGT6、LQFP-144；若直接沿用，会导致器件封装、引脚和生成工程错误。

### 复现步骤

比较原 README 中的目标 MCU 与开发板芯片丝印、原理图和厂商资料。

### 已知事实

- 实物芯片为 STM32F407VGT6。
- 原理图中的 MCU 为 STM32F407Vx，封装为 LQFP-100。
- VGT6 与 ZGT6 的引脚数量和封装不同。

### 初始假设

M0 使用了尚未经过实物与原理图确认的初始目标型号。

### 验证过程

交叉核对芯片丝印、开发板原理图、引脚分配图、厂商资料和 ST 数据手册，并搜索仓库中的型号引用。

### 根本原因

在硬件信息尚未确认时，将初始目标型号写入了项目基线。

### 解决方案

将当前目标统一为 STM32F407VGT6、LQFP-100；在项目约束中明确禁止使用 ZGT6 代替，并把准确器件选择设为 CubeMX 的第一项检查。

### 验证结果

当前约束、README、状态、决策与测试记录均以 VGT6 为实际目标；ZGT6 只保留在错误历史和禁止误选的说明中。

### 为什么会犯这个错误

在进入硬件核对步骤前，过早固化了未验证的型号信息。

### 如何避免再次发生

生成工程前必须交叉核对芯片丝印、完整订货型号、封装、原理图和 CubeMX 器件选择。

### 我学到了什么

STM32 完整订货型号中的封装与容量字符会直接影响引脚和工程配置，不能只核对“STM32F407”系列名。

## 记录模板

```markdown
## ERR-YYYYMMDD-XX：标题

### 日期

### 模块

### 当前环境

### 问题现象

### 期望行为

### 实际行为

### 复现步骤

### 已知事实

### 初始假设

### 验证过程

### 根本原因

未确认时明确写“未确认”。

### 解决方案

### 验证结果

### 为什么会犯这个错误

### 如何避免再次发生

### 我学到了什么
```

记录时保留真实排查过程，明确区分事实、假设、已证实和未证实内容。

## ERR-20261016-01：Ring Buffer 消费者未检查读取状态导致旧数据重复打印

### 日期

2026-10-16。

### 模块

M4 Ring Buffer / FreeRTOS Consumer Task / Debug Logger。

### 当前环境

- STM32F407VGT6。
- FreeRTOS + CMSIS-RTOS2。
- USART2 同步日志。
- 临时复用 SemProducerTask / SemConsumerTask。
- Ring Buffer 使用固定容量数组和满时拒绝写入策略。

### 问题现象

初始 Ring Buffer 测试中，串口日志出现同一数据值被连续输出的情况。

例如：

```text
[I][RIB] value acquired: 14
[I][RIB] overflow turn: 1
[I][RIB] value acquired: 14
```

### 期望行为

- 读取成功时输出本次读取的数据。
- 读取失败时不将旧变量值打印为新数据。
- overflow_count 只表示历史溢出写入次数，不决定当前可读取数据数量。

### 实际行为

- Consumer 未检查 RingBuffer_ReadByte 返回状态。
- 即使读取失败，也可能继续打印 value_acquired 的旧值。
- overflow_count 大于 0 后，Consumer 额外执行读取。
- 日志存在误导性重复数据。

### 复现步骤

1. 使用初始 Ring Buffer Consumer 测试代码。
2. 连续运行 Producer / Consumer。
3. 使 Ring Buffer 出现一次 Full 写入失败。
4. 保持 overflow_count 大于 0。
5. Consumer 每轮执行额外读取。
6. 在缓冲区数据不足时观察重复输出旧值。

### 已知事实

- RingBuffer_ReadByte 使用返回状态区分读取成功与 Empty。
- Empty 读取失败时不应修改输出数据。
- overflow_count 为累计历史统计。
- 初始 Consumer 没有完整检查 ReadByte 返回值。
- 修改后重复打印现象未再出现。

### 初始假设

可能是 Ring Buffer FIFO 顺序或索引回绕存在错误。

### 验证过程

- 检查 Consumer 读取代码。
- 发现读取后立即打印，没有判断返回状态。
- 发现使用 overflow_count 大于 0 作为额外读取条件。
- 将数据变量统一为 uint8_t。
- 增加 RingBufferStatus_t 返回状态判断。
- 只有 RING_BUFF_OK 时才打印读取数据。
- 不再使用累计 overflow_count 控制额外读取次数。
- 后续完成 Empty / Full / Wrap-around 确定性测试。
- 五个测试阶段均输出 PASS。

### 根本原因

Consumer 将累计溢出历史误当作当前缓冲区数据状态，并且没有检查读取函数的返回值。

读取失败后，旧输出变量值仍被打印，使日志看起来像重复读取了相同数据。

### 解决方案

- 每次调用 RingBuffer_ReadByte 后检查返回状态。
- 只有读取成功才使用输出数据。
- Empty 作为正常边界单独处理。
- overflow_count 仅用于统计和告警，不决定读取次数。
- 使用返回状态和确定性测试验证实际行为。

### 验证结果

- 修改后未再观察到原有重复旧值日志。
- Empty 确定性测试 PASS。
- Full 确定性测试 PASS。
- FIFO 和 Wrap-around 确定性测试 PASS。
- Ring Buffer 功能 checkpoint：`f8c3e52`。

### 为什么会犯这个错误

- 将函数调用与操作成功混为一谈。
- 没有先检查返回值再使用输出参数。
- 没有区分历史溢出统计与当前数据可用状态。
- 在测试阶段过度依赖日志内容而没有验证日志对应的操作状态。

### 如何避免再次发生

- 所有可能失败的数据读取 API 必须检查返回状态。
- 读取失败后不得将旧值当作新数据处理。
- 为 Full / Empty / Timeout / Invalid Argument 建立明确的处理分支。
- 数据丢失计数与 Buffer 当前状态分别管理。
- 对异常路径进行确定性测试。
- 日志必须准确反映操作成功或失败。

### 我学到了什么

- 工业采集系统中，错误日志本身可能造成错误的数据判断。
- 不能只根据数值输出判断 FIFO 是否正确。
- 返回状态和输出数据必须一起验证。
- 累计统计量不能直接代表当前 Buffer 状态。
- 确定性测试比依赖任务运行时序更容易定位边界错误。


## ERR-20261018-01：CubeMX 生成 CRLF 导致 Git 大量无关文件显示修改

### 日期

2026-10-18（M4.5 TIM7 代码生成和排查期间）。

### 模块

CubeMX / Git / EOL 规范 / FreeRTOS 中间件文件。

### 当前环境

Windows PowerShell、CubeMX、Git；仓库内源码使用 `text eol=lf`，`core.autocrlf=false`。

### 问题现象

CubeMX Generate Code 后，`git status --short` 出现大量中间件、HAL 与工程文件被修改，`git diff` 提示 `CRLF will be replaced by LF`。

### 期望行为

只显示 TIM7 相关的 `.ioc`、CMake、生成外设源码和少量 RTOS 集成变更，不产生几十个 EOL-only 噪音文件。

### 实际行为

`git ls-files --eol` 证实典型文件为 `i/lf w/crlf attr/text eol=lf`；最终恢复后 Diff 收敛至 TIM7 必需修改，`git diff --check` 无输出。

### 复现步骤

在仓库要求 LF 的工程中由 CubeMX 重新生成代码，执行 `git status --short`、`git diff --stat` 并检查 `git ls-files --eol`。

### 已知事实

- `Src/freertos.c`、`Src/main.c`、`.ioc`、`stm32f4xx_it.c` 曾显示 `i/lf w/crlf attr/text eol=lf`。
- `core.autocrlf=false`。
- 正常化后保留了 TIM7 新文件 `Inc/tim.h`、`Src/tim.c`、`stm32f4xx_ll_tim.h`，未丢失功能修改。

### 初始假设

CubeMX 将工作区原 LF 文件重新写为 CRLF，造成 Git 检测噪音。

### 验证过程

对照 `git diff --ignore-space-at-eol --stat`、`git ls-files --eol` 和普通 Diff；为修改文件备份后按字节将 CRLF 改为 LF，重新执行 `git status`、`git diff --stat`、`git diff --check`。

### 根本原因

CubeMX 生成文件换行格式与仓库 LF 约束不一致；Git 工作区状态包含行尾变化噪音。

### 解决方案

仅处理确需 LF 的文件，保留实际 TIM7 生成差异，不强制全库重整；对 `LICENSE` 等内容不变的文件按确认后的原版本恢复。

### 验证结果

- 大量 FreeRTOS 中间件文件从修改列表消失。
- `git diff --stat` 与 `git diff --ignore-space-at-eol --stat` 一致。
- `git diff --check` 无输出。
- 后续干净构建通过，M4.5 板级实验通过。

### 为什么会犯这个错误

未预期 CubeMX 的 Windows 文本输出可能与仓库 LF 规范冲突，最初将批量显示的文件误当成大量业务修改风险。

### 如何避免再次发生

每次 Generate Code：先备份 → 检查 `git ls-files --eol` → 定向规范化 → 审核实际 Diff → Clean Build，避免未经审查的全量暂存或强制回退。

### 我学到了什么

`git status` 的“被修改”与实质性语义改变不是一回事；版本控制的行尾规则应与代码生成工具一起考虑。

## ERR-20261018-02：TIM7 .ioc 的 Period 已设为 999，但 tim.c 暂时仍为 9

### 日期

2026-10-18。

### 模块

STM32CubeMX / TIM7 / 代码生成同步。

### 当前环境

TIM7 定时器时钟 84 MHz，PSC=8399；目标更新周期 100 ms。

### 问题现象

`STM32F407_Industrial_Platform.ioc` 保存 `TIM7.Period=999`，但当时 `Src/tim.c` 仍生成 `htim7.Init.Period = 9`。

### 期望行为

`Src/tim.c` 中 ARR 为 999，周期 100 ms / 10 Hz。

### 实际行为

ARR=9 时周期仅为 1 ms / 1000 Hz；与实验设计不符。

### 复现步骤

对照 `.ioc` 的 `TIM7.Period` 与 `Src/tim.c` 中 `htim7.Init.Period`，检查两文件修改时间。

### 已知事实

- `.ioc`：`TIM7.Period=999`、`TIM7.Prescaler=8399`。
- 曾查看到 `tim.c` 中 Period=9。
- `.ioc` 修改时间 22:44:13，比 `tim.c` 当时的 22:42:52 晚。

### 初始假设

配置保存与代码生成先后不同步，或生成文件尚未更新；时间先后是线索，不单独证明生成器内部原因。

### 验证过程

重新 Generate Code 后用 PowerShell 检查 `Src/tim.c`，最终确认 `htim7.Init.Prescaler = 8399`、`htim7.Init.Period = 999`。

### 根本原因

**未从 CubeMX 内部日志独立证实。** 已观察到配置与生成文件曾不同步，重新生成消除了不一致。

### 解决方案

以 `.ioc` 为配置源，保存配置后重新 Generate Code；立即对照源文件值，再处理行尾差异并构建。

### 验证结果

最终 `Src/tim.c` 的 Period=999；已通过 M4.5 板级 IRQ → Task 通信实验。

### 为什么会犯这个错误

当时未在生成后第一时间核对关键定时器 ARR 与目标周期。

### 如何避免再次发生

每次修改定时器：计算时钟 → 检查 `.ioc` → 重新生成 → 检查 `tim.c` → clean build → 实机观测。

### 我学到了什么

`.ioc` 正确并不自动证明工作区的自动生成 C 文件已与其同步；周期应同时通过算式和实测链路核验。


## ERR-20261028-01：M5-P0 CubeMX 代码再生成再次造成大量 CRLF/LF Git 噪音

### 日期

2026-10-28。

### 模块

CubeMX Generate Code / Git 工作区 / FreeRTOS 中间件与项目自动生成文件。

### 当前环境

从 `e793039` 开始的 `feature/m5-adc-timer-dma-baseline`，在 `.ioc` 中删除 M4 实验任务和同步对象后重新生成工程；仓库历史采用 LF 规范，CubeMX 在 Windows 下生成了多份 CRLF 工作区文件。

### 问题现象

`git status --short` 曾显示 57 个修改文件，包含大量未主动修改的 FreeRTOS 第三方源码；`git diff --check` 仅连续提示 `CRLF will be replaced by LF`，影响真实功能修改审核。

### 期望行为

Git Diff 仅显示本轮任务删减与接口调整的实际修改，第三方 FreeRTOS 源码和无实质变化的生成文件不进入提交。

### 实际行为

57 个文件先被标记修改；用户恢复第三方 FreeRTOS 内容后，工作区还剩 22 个修改文件；继续审核及恢复 EOL-only 文件后剩 11 个修改文件，其中忽略行尾空白后只有 7 个文件存在内容差异。

### 复现步骤

在 Windows 中使用 CubeMX 重新生成代码，再运行 `git status --short`、`git diff --ignore-space-at-eol --name-only`、`git diff --check`，观察多余文件修改与 EOL 转换警告。

### 已知事实

- `git diff --ignore-space-at-eol --stat` 最终显示 7 个实质差异文件、28 行新增 / 938 行删除，符合删除式重构。
- TIM6/TIM7 的三个生成源文件在忽略行尾空白后没有代码差异；`.ioc` 有真实的 FreeRTOS 任务/同步对象移除。
- `git -c core.safecrlf=false diff --check` 没有报告新增行尾空格/空白错误。
- 未收到最终只剩 7 个修改文件以及新 Commit / Push 的终端结果。

### 初始假设

与 2026-10-18 的已记录故障相同：CubeMX 新生成的 CRLF 工作区内容与 Git 历史 LF 格式不一致，而非大量 FreeRTOS 内核业务代码实际被改写。

### 验证过程

分组比较普通 Diff、`--ignore-space-at-eol` Diff 与文件列表；只对明确没有需要保留的业务差异的第三方文件或 EOL-only 文件执行 `git restore --worktree`，保留 `.ioc` 和业务源文件。使用 `git diff --check` 核查空白错误。

### 根本原因

本次大量修改显示主要由生成文件的行尾格式变化引起；未附 CubeMX 内部执行日志，不能进一步断言工具内部具体转换步骤。

### 解决方案

在确认无有效代码差异后定向恢复不需要提交的文件；重点保留与审核 `m4_ipc`、`m4_isr`、`freertos.c`、`main.c` 和 `.ioc`；不得使用 `git reset --hard` 或不审查就全量暂存。本次诊断使用的 `-c core.safecrlf=false` 不意味着永久关闭 Git 安全检查。

### 验证结果

已减少到 11 个工作区修改文件、7 个实质差异文件；Debug 增量构建与板级运行测试均已通过。最终 Git 状态 / Commit 仍待确认，本条问题按“主要噪音已隔离、提交前复核待完成”记录。

### 为什么会犯这个错误

CubeMX 再生代码可能把未主动编辑的文件重新写成另一种行尾格式；仅观察 Git 修改数量容易把格式差异误判为批量功能变更。

### 如何避免再次发生

生成前建立干净基线并备份；生成后先查看 `git status`、`git ls-files --eol`、忽略行尾 Diff 与普通 Diff，区分真实配置变化和 EOL-only 差异；提交前再检查暂存区和 `git diff --cached --check`。

### 我学到了什么

行尾提示与代码错误并不等价；只有把 CubeMX 配置源、生成代码、Git 索引及真实板级回归相互核对，才能得到可审计的功能提交。

## 2026-10-29 M5 开发错误记录状态补充

- M5-P1～P5 的已报告构建、ADC 电压、Half/Full 与回归日志中未出现已确认且未解决的新固件故障；不因故意注入的 250 Tick 延时和 Late 增加而虚构新的 ERR 编号。
- 2026-10-28 M5-P0 的 CubeMX CRLF/LF 噪声已在后续 Git 审核后被排除，M5-P0 功能 `942fcc3`、文档 `e7f525c` 均已提交并推送；上方 `ERR-20261028-01` 中“待确认 Git 提交”属于当时的历史记录，现由此补充关闭。
- 新的 ADC/TIM2/DMA 功能版本尚未提供对应的最终 Git 变更清单或提交结果；若 CubeMX 生成时再出现大面积 EOL-only Diff，继续沿用 `ERR-20261018-01` / `ERR-20261028-01` 的审核策略。
- DMA Circular 潜在旧数据覆盖与 Thread Flags 通知合并目前记录在 `KG-014` 及 `TEST-20261029-01` 的设计/验证边界中，不能未经证据就登记为已复现固件 Bug。

## ERR-20261101-01：M6 CubeMX 再生成后大量 CRLF/LF 状态噪音

### 日期

2026-11-01。

### 模块

CubeMX 生成代码 / Git 工作区 / `.gitattributes` 换行符规范。

### 当前环境

`feature/m6-baseline`（从 `30b0072` 开始）；M6-P1 新增 USART2 RX DMA 与对应 IRQ；仓库文本源码要求 LF，CubeMX 在 Windows 下写出 CRLF 工作区文件。

### 问题现象

`git diff --stat` 曾列出 15 个文件、91 行新增/3 行删除；大量文件出现 `CRLF will be replaced by LF` 警告。清除非预期 Diff 后只剩 5 个功能文件，但 `git status -sb` 仍把 FreeRTOS 中间件等许多文件标记为 `M`。

### 期望行为

只保留本阶段 `.ioc`、DMA/UART 和 IRQ 的真实配置修改，不混入无关 HAL/FreeRTOS 行尾变化。

### 实际行为

恢复后仍出现 CRLF 警告和大量 EOL-only 工作区修改；经过保留差异的后续处理，最终 `git diff --stat` 仅 5 个预期文件 `78 insertions(+), 1 deletion(-)`，`git diff --check` 无输出，`git status -sb` 仅这 5 个 `M`。

### 复现步骤

在 Windows 上使用 CubeMX 重新生成 M6 工程后执行 `git diff --stat`、`git diff --check`、`git status -sb`；比较 `git diff --ignore-space-at-eol --stat` 与真实源码 Diff。

### 已知事实

- 绝大部分被标记 `M` 的 FreeRTOS 文件没有普通 Diff 内容变化。
- 需保留的五个文件为 `Inc/stm32f4xx_it.h`、`STM32F407_Industrial_Platform.ioc`、`Src/dma.c`、`Src/stm32f4xx_it.c`、`Src/usart.c`。
- 最后确认的 `git diff --check` 没有空白错误；此为 M6-P1 处理结束时快照。

### 初始假设

CubeMX 输出 CRLF 与 Git LF 工作区策略不一致，而非第三方中间件被实质修改。

### 验证过程

用户按差异审查与恢复建议整理文件；最后回传 `git diff --stat`、`git diff --check`、`git status -sb`，验证仅保留 5 个 M6 配置文件。

### 根本原因

工作区行尾与 Git 文件属性要求不一致；未提供生成器内部执行细节，故不进一步归因至某项 CubeMX 内部转换实现。

### 解决方案

保留 M6 有效 Diff，按文件确认 EOL-only 改动后安全恢复/清理工作区；不得使用不留备份的 `git reset --hard` 或未经审核的全量暂存。

### 验证结果

**已解决（M6-P1 工作区噪音）**：5 个预期文件、无 `git diff --check` 错误；后续 M6-P2 业务代码的当前 Git 状态需另行确认。

### 为什么会犯这个错误

CubeMX 重新生成可能同时改变未主动编辑文件的换行符；`git status` 的 `M` 不必然代表业务代码变化。

### 如何避免再次发生

生成前确认干净基线，生成后逐步比较普通/忽略行尾 Diff 与 `git ls-files --eol`，保留补丁或文件备份，再恢复仅换行符变化的文件。

### 我学到了什么

Git 警告、工作区修改标志、可提交的实际代码差异是三个不同层次，需要分别核对。

## ERR-20261101-02：UART 正常 RxEvent 被误记录为硬件 Error

### 日期

2026-11-01。

### 模块

M6-P2 HAL UART 接收回调 / 错误统计。

### 当前环境

USART2 DMA Normal + IDLE，`HAL_UARTEx_RxEventCallback()` 负责有效数据入队，另有错误分类计数函数。

### 问题现象

普通数据收发过程中 `[RXE] Total` 与正常数据处理次数一起增加，而 `Last=0x00000000`、FE/ORE 分类没有新增真实硬件错误。

### 期望行为

普通 IDLE/TC 接收不增加硬件错误计数；只有收到非零 `HAL_UART_ERROR_*` 时才记录对应分类。

### 实际行为

初版将错误检测/记录函数放在正常接收回调内调用，导致 `Total` 统计函数调用次数而不是错误次数。

### 复现步骤

不做真实硬件错误注入，仅正常发送串口数据，观察 `[RXM] Count` 和 `[RXE] Total` 一同增加、`Last=0`。

### 已知事实

- 用户明确定位到误调用位置，并确认已调整。
- 关闭模拟 FE/ORE 注入后，正常 ASCII `01 02 03 04` 共 11 次，收到 11×11 B，对应 `Count=11 Drop=0 StartFail=0`，每次 `[RXE] Total=0 ... Last=0x00000000`。
- 压力测试的所有可见 `[RXE]` 仍为零。

### 初始假设

曾误将增长的 `Total` 理解为真实 HAL ErrorCallback 次数，实际该值来自正常回调中的记录函数调用。

### 验证过程

移除错误检测函数在正常回调中的调用；临时软件自检不参与正式测量；重新复位、发送 11 次正常 ASCII，核对每次错误分类与最后错误码。

### 根本原因

混淆“正常接收完成事件”和“UART 真实错误事件”，错误统计的调用入口设计不正确。

### 解决方案

错误回调与 RxEvent 回调分离，只在真正的 UART 错误报告且 ErrorCode 非零时进行错误分类计数；ISR 不调用阻塞日志。

### 验证结果

**已解决（正常接收误计数）**；真实 FE/ORE 硬件错误注入及错误后恢复没有因此自动获得验收。

### 为什么会犯这个错误

没有先区分 HAL 的接收事件类型与错误事件来源，就复用了同一个计数函数。

### 如何避免再次发生

统计变量命名体现物理含义；分别跟踪 RxEvent、ErrorCallback、ErrorCode、队列失败与 DMA StartFail，并用无错误正常收发作为负向对照。

### 我学到了什么

`TotalError=0` 只能说明当前错误监控没有记录错误，不等于传输过程没有数据丢失。

## ERR-20261101-03：64 B Normal DMA 长流压力下 8 格消息队列入队失败

### 日期

2026-11-01。

### 模块

M6-P2 DMA RX / CMSIS-RTOS2 Message Queue / UARTTask 消费速度。

### 当前环境

USART2 115200/8N1，Normal DMA 缓冲 64 B；UARTTask 周期处理并输出调试日志；先后将消息队列深度设为 8 和 64 进行相同文本数据对照。

### 问题现象

8 格队列压力下 `Drop` 逐步增加到 49，`Count` 末次为 747；没有新增 HAL 错误和接收启动失败。64 格队列相同条件下最后 `Drop=0`。

### 期望行为

高吞吐输入的缓存溢出有可见计数；如果要求端到端无损，应用层有效处理字节应和已知发送数据匹配。

### 实际行为

8 格队列确认有 49 次 `osMessageQueuePut` 失败；64 格本轮未观察到入队失败，但不能排除 Normal DMA 重启空窗内的无计数漏字节。

### 复现步骤

在串口工具使用 ASCII 模式，重复发送由 `01 02 ... 10` 文本组成的长序列；先用 8 格队列，再改为 64 格；对比 `[RXM] Count/Drop/StartFail` 和 `[RXE]`、Free Heap。

### 已知事实

- 每组 13 条长 ASCII 发送记录，每条日志文本 3599 字符；两组发送文本规模和模式可比，但 `Count` 起点与 PC 实际串行发送时序不能直接作为严格字节损失率比较。
- 8 格：`Count=747 Drop=49 StartFail=0`，Heap/MinHeap=`11704 B`，UARTFree 最低 344 B。
- 64 格：`Count=753 Drop=0 StartFail=0`，Heap/MinHeap=`7672 B`，UARTFree 最低 336 B。
- 两组 `[RXE] Total/PE/NE/FE/ORE/DMA` 均为 0，Last=`0x00000000`。

### 初始假设

队列瞬时缓存不足或消费任务响应不及时可导致入队失败；Normal DMA 重启期间还可能独立存在不报 FE/ORE 的字节损失。

### 验证过程

仅调整消息队列深度从 8 至 64，复用同类发送负载，比较 `Drop` 与 Free Heap；扩容后 Drop 归零且 Free Heap 减少 4032 B（约 72 B/新增元素）。

### 根本原因

已确认的直接丢弃点是 8 格队列满时入队失败；导致瞬时积压的各项具体时序占比尚未量化。队列容量不足不能解释或排除所有潜在的 DMA 重启窗口丢字节。

### 解决方案

将 64 格作为诊断对照，不无限扩容；进入 M6-P3 改为 Circular DMA、独立字节 Ring Buffer 和及时消费，分别统计接收、缓存、消费及溢出字节数。

### 验证结果

**8 格压力条件下确认发生入队失败；64 格组此条件下观察 Drop=0**。本项尚未完成端到端连续流无损验收，转入 M6-P3。

### 为什么会犯这个错误

把“每次 DMA 回调能够入队”与“任务能长期跟上上游速率”混为一谈，低负载正常不代表高负载队列容量足够。

### 如何避免再次发生

设计数据通道时区分峰值生产速率、平均消费速率、缓存容量与调试日志阻塞；用已知发送序列及总字节数做真正的完整性测试。

### 我学到了什么

`Drop` 记录的是应用层明确失败事件；硬件 Error=0、`StartFail=0` 和队列扩容后的 Drop=0 都不能单独证明零丢字节。
