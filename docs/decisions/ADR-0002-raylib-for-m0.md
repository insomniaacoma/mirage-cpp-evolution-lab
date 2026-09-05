# ADR-0002: M0 采用 raylib 5.5 作为窗口/输入/绘制层

- Date: 2026-09-06
- Status: accepted
- Supersedes: 无
- Superseded By: （空）
- Refs: RFC-0002, ISS-002, RUN-0001
- **Autonomous Project Decision**: yes（Owner 未裁决，依 RUN 协议 §3 由团队决策；可被 Owner 未来 ADR supersede）

## Context

M0 需要最小窗口/输入/绘制能力；M0 的目的是玩法验证而非图形学习。环境：MSVC x64，GitHub 经本地代理可达（直连不稳）。

## Decision

1. 引入 **raylib 5.5**（官方 MSVC 预编译静态包）。
2. 获取方式：`tools/fetch_raylib.py`（标准库实现，走代理）下载解压到 `third_party/raylib/`；该目录 **gitignored**（不提交二进制，可复现重建）。
3. **调用面收敛**：raylib API 只允许出现在 `src/main.cpp` 与 `src/scene.cpp`。这条纪律是"将来可替换"从口号变成事实的唯一保证（Rin 的 review 条件）。
4. 静态链接；不引入 DLL。

## Alternatives

- GLFW+glad+OpenGL 3.3：学习价值高但成本 3–5 倍，保留为后续学习路径（不阻塞 M0）。
- SDL2：能力重叠且更底层；Win32+GL：纯手写税。均不选。

## Consequences

- 正面：M0 数天内可玩；构建链最短；`scene.cpp` 成为将来真实渲染模块的天然替换点（Rin 的入场位）。
- 负面 / 显式接受的债：raylib 调用面代码在渲染模块出生时**整体抛弃**；raylib 的即时模式风格可能诱导把游戏状态塞进绘制循环——review 时盯住（Rin watchpoint）。
