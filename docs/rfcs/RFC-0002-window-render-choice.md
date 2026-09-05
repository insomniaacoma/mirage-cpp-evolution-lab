# RFC-0002: M0 的窗口/渲染方案

- Status: accepted
- Authors: Arden
- Reviewers: Rin, Vera, Nico
- Created: 2026-09-06（RUN-0001 · P1 · logical）
- Decision: ADR-0002
- Decision marker: **Autonomous Project Decision**（Owner 未裁决；证据充分，团队依协议 §3 自行决策；Owner 可随时以新 ADR supersede）

## Context

M0 需要窗口 + 输入 + 简单几何绘制。这是项目第一个第三方依赖（T2）。环境实测（P0）：VS2022 MSVC 可用，GitHub 经本地代理可达。

## Problem

在 M0 时间盒内拿到"能开窗口、能画盒子、能读键鼠"的最小能力，且选择本身可被记录、可被抛弃。

## Constraints

- M0 时间盒：目标是玩法验证，不是图形学习。
- 依赖必须显式经 ADR（charter P1）。
- 工具链：MSVC x64（ISS-001 裁决）。
- 必须能在离线/代理环境下可复现获取。

## Proposal

**raylib 5.5**（官方 MSVC 预编译包），经 `tools/fetch_raylib.py` 下载解压至 `third_party/raylib/`（gitignored），静态链接。调用面收敛在 `src/scene.cpp` 与 `src/main.cpp`。

## Alternative A — GLFW + glad + OpenGL 3.3

学习价值最高（Owner 想深入管线）。但窗口/输入/着色器全手工，M0 成本约 3–5 倍，与验收标准无关。**未被拒绝的学习路径**：留作后续 milestone 或沙盒项目。

## Alternative B — SDL2 / Win32+GL 裸写

SDL2 与 raylib 能力重叠但更底层；Win32+GL 纯手写对 M0 纯粹是税。均不选。

## Trade-offs

得到：最快到可玩；单一静态库；构建简单。
放弃：图形管线学习；将来换自研渲染层时 raylib 调用面代码是**抛弃式的**——此债显式记录于 ADR-0002。

## Migration

无（首依赖）。退出路径：渲染模块出生时（Rin 的 RFC）以新渲染层替换 `scene.cpp`/`main.cpp` 内的调用面。

## Risks

- 预编译包与 MSVC 工具链 ABI 不匹配 → P1 spike 实测消除（已通过，见 build 日志）。
- 代理下载不可复现 → fetch 脚本固定版本号 + 校验文件存在性。

## Open Questions

无（M0 范围内）。

## Reviewers

- **Rin**（rendering）：倾向 GLFW+GL 的学习价值，**接受 raylib**——条件是把"调用面收敛"写进 ADR，让将来的替换是真的可行而不是嘴上说。
- **Vera**（core）：无异议；要求静态链接、不引入 DLL 地狱。
- **Nico**（gameplay）：谁快用谁，别耽误我写 mutation。

## Decision

Accepted → [ADR-0002](../decisions/ADR-0002-raylib-for-m0.md)
