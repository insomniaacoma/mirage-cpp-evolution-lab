# ISS-001: Repository bootstrap — git、工具链、CMake 骨架

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | infra |
| Owner | **owner** 🎓（Mora 提供支持） |
| Reviewer | Dee |
| Status | open |
| Depends on | — |

## 背景

仓库目前只有文档，还不是 git 仓库。M0 所有后续 Issue 都依赖一个能构建的骨架。这是 Owner 的第一个亲手任务 🎓：环境与构建是一切的地基，值得亲手搭一次。

## 需求 / 验收标准

- [ ] `git init` 并完成首次 commit（包含全部文档与 `.gitignore`）。
- [ ] 确定工具链并写入 `docs/build.md`：编译器（建议 MSVC / VS 2022 Build Tools）、CMake ≥ 3.25、生成器（建议默认 Visual Studio，调试简单）、C++ 标准（建议 C++20）。
- [ ] 根 `CMakeLists.txt` 定义唯一 target `mirage_m0`：一个 `src/main.cpp` 打印 `MIRAGE M0` 后正常退出。
- [ ] `cmake -B build && cmake --build build` 一次成功；把确切命令写进 `docs/build.md`（新人照抄可复现）。
- [ ] 追加 observatory 事件：`commit.recorded`、`issue.status_changed`。

## Non-goals

不建 CI（Dee：没有可自动验证的东西）；不选渲染库（ISS-002）；不拆任何模块；不加测试框架。

## Notes（学习目标）

- 为什么单 target、`src/` 扁平是**现在**的正确选择，而不是偷懒（charter P0/P1：压力出现前不建模块）。
- 工具链三件套（编译器 / 生成器 / 标准版本）各自影响什么；MSVC 与 MinGW 的差异对后续 milestone 的含义。
