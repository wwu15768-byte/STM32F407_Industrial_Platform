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
