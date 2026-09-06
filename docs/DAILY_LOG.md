# 每日开发日志

> 唯一回答：今天实际上干了什么？
> 更新时机：每个有效开发日结束时，简洁记录事实。

# 2026-09-06

## 今日计划

- 确认开发环境。
- 初始化 Git 并建立忽略规则。
- 建立 README 与 docs 项目管理体系。
- 为第一次稳定提交做好准备。

## 实际完成

- 确认 STM32CubeMX + CMake + Ninja + GNU Arm Toolchain + CubeProgrammer/ST-LINK 工具链可用。
- 初始化本地 Git 仓库，设置默认分支 `main`，确认提交者配置。
- 创建并验证 `.gitignore`。
- 完成 README 编写与内容复核。
- 建立并验收 8 个 docs 管理文件。
- 创建第一次稳定提交 `8bc8d98`。
- 建立 GitHub 私有远程仓库，配置并推送 `origin/main`。
- 通过 `git ls-remote` 验证 GitHub 远程 `main` 的提交哈希。
- 确认旧提交保留旧邮箱，后续提交使用 Gmail 身份。

## 验收情况

- 开发环境：已验证。
- Git 初始化：已验证。
- `.gitignore`：已通过示例路径验证。
- README：已复核通过。
- docs：已完成内容验收。
- 第一次稳定提交：已创建并完成提交后检查。
- 远程仓库：已完成首次推送和远程哈希验证。
- M0：验收通过。

## 未完成

- M1 尚未开始。

## 今日 Bug

无已确认项目 Bug。

## 今日学习

- Git 仓库、工作区和忽略规则的基本作用。
- `.gitignore` 对未跟踪文件生效，不会自动取消已跟踪文件。
- Git 全局配置与仓库配置的作用范围不同。
- `git ls-remote` 会直接读取远程引用，`origin/main` 是本地保存的远程跟踪引用。
- 修改 Git 身份不会追溯修改已有提交。

## 尚未理解

- 暂存区与提交快照的完整检查流程仍待实际操作。

## Git

- Branch：`main`
- Commit：`8bc8d98`，以及本次 M0 收尾文档提交
- Remote：`origin/main`

## 明天下一步

进入 M1，先确认硬件型号、时钟源、LED、UART 和 ST-LINK 连接信息。
