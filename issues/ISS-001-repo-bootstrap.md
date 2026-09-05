# ISS-001: Repository bootstrap — git、工具链、CMake 骨架

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | infra |
| Owner | **owner** 🎓（Mora 提供支持）→ 剩余项转 Agent 实现（RUN-0001 · SD-1） |
| Reviewer | Dee |
| Status | done（RUN-0001 · P1） |
| Depends on | — |

## 进展

- 2026-09-06：`git init`（main）+ 首次 commit（`f0eef0b`）+ GitHub 远端（private，由 Owner 委托本次会话完成；仓库级 git 身份 = insomniaacoma 的 noreply 邮箱）。**工具链选型、CMake 骨架、`docs/build.md` 仍是 🎓 Owner 亲手任务。**
- 2026-09-06：已推送到 `origin/main`（`6f0ecfd`），托管完成。直连 github.com 间歇性中断，本仓库配置了仅对 github.com 生效的代理 `http://127.0.0.1:7897`（repo-local，不影响全局配置）。

## 背景

仓库目前只有文档，还不是 git 仓库。M0 所有后续 Issue 都依赖一个能构建的骨架。这是 Owner 的第一个亲手任务 🎓：环境与构建是一切的地基，值得亲手搭一次。

## 需求 / 验收标准

- [x] `git init` 并完成首次 commit（包含全部文档与 `.gitignore`）。
- [x] 确定工具链并写入 `docs/build.md`：编译器（建议 MSVC / VS 2022 Build Tools）、CMake ≥ 3.25、生成器（建议默认 Visual Studio，调试简单）、C++ 标准（建议 C++20）。
- [x] 根 `CMakeLists.txt` 定义唯一 target `mirage_m0`：一个 `src/main.cpp` 打印 `MIRAGE M0` 后正常退出。
- [x] `cmake -B build && cmake --build build` 一次成功；把确切命令写进 `docs/build.md`（新人照抄可复现）。
- [x] 追加 observatory 事件：`commit.recorded`、`issue.status_changed`。

## Non-goals

不建 CI（Dee：没有可自动验证的东西）；不选渲染库（ISS-002）；不拆任何模块；不加测试框架。

## 状态注记（RUN-0001 · P1 · SD-1）

- Owner 已完成 git init / 托管；剩余项（工具链、CMake、build.md）依 RUN 协议 §4.8 由 ver/vic 完成，🎓 转 Learning Hook：`docs/learning/RUN0001-P1-cmake-toolchain.md`。
- 工具链裁决：MSVC（VS2022）+ VS 自带 CMake 3.31.6 + C++20；raylib 5.5 经 `tools/fetch_raylib.py`（代理）获取并以 `.spike/` 验证静态链接（exit 0）。
- 证据：`build/Release/mirage_m0.exe` → `MIRAGE M0`，exit 0；构建命令已写入 `docs/build.md`。

## Notes（学习目标）

- 为什么单 target、`src/` 扁平是**现在**的正确选择，而不是偷懒（charter P0/P1：压力出现前不建模块）。
- 工具链三件套（编译器 / 生成器 / 标准版本）各自影响什么；MSVC 与 MinGW 的差异对后续 milestone 的含义。
