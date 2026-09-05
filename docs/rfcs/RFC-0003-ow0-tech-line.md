# RFC-0003: OW0（Observatory Wave 1）技术线

- Status: accepted
- Authors: Mora
- Reviewers: Arden, Ash, Dot
- Created: 2026-09-06（RUN-0001 · P1 · logical）
- Decision: ADR-0003
- Decision marker: Autonomous Project Decision

## Context

OW0 目标从"数据模型"升级为**可运行、可点击、可回放**的观测站 v1：Studio / Architecture / Work / Timeline / Discussions / Metrics / Process Lab。约束：现有事件日志（JSONL）+ checkpoints 是事实源；工具链政策为"标准库优先"（ISS-008 先例）；必须今天就能跑起来。

## Problem

选一条 UI 技术线，在零额外依赖约束下最快交付七视图 + 时间轴回放。

## Proposal — 方案 C：静态 Web UI + Python snapshot generator

`tools/obslog.py view` 读取全部事件 + checkpoints → 计算投影 → 生成 `observatory/view/data.js`（`window.OBS_DATA`）；`observatory/view/index.html + app.js` 手写 SVG/Canvas 渲染七视图，零外部依赖，file:// 可直接打开。数据块显式带 `source: "repository" | "fixture"`。

## Alternative A — Dear ImGui（纯 C++ GUI）

C++ 学习价值高、与游戏同栈。但：引入 imgui + 后端两套依赖；七视图的图布局/时间轴全手工；开发速度最慢；且 OW0 是产品不是练习场。不选。

## Alternative B — C++ Core + Web 前端（本地 server）

交互能力最强。但：两个运行时 + 进程间协议，M0 同期交付不现实。留作 OW1+ 演进路径（若 C 方案碰到真实压力，这正是下一次演化的候选方向——记录在案）。

## Trade-offs

得到：零依赖、最快、回放模型天然匹配（数据 = 事件投影的纯函数）。
放弃：实时性（数据需重新生成 `data.js`）；复杂交互。可接受：观测站的更新节奏本来就是 checkpoint 级。

## Migration

无。演进路径：C → B（加本地 server 热更新）由真实压力触发，届时走新 RFC。

## Risks

- 手写 SVG 图布局工作量被低估 → 视图先做"可读"再做"好看"；Studio 用固定房间网格，Architecture 用简单力导向或固定布局。
- data.js 膨胀 → 只导出投影而非全量 payload。

## Open Questions

- Code Evolution View（checkpoint A→B diff）OW0 是否必做 → Wave Review 裁定（协议标注"保留模型即可"）。

## Reviewers

- **Ash**（architecture junior）：建议投影在 generator 侧完成，前端只画——否则两处逻辑漂移。采纳。
- **Dot**（QA junior）：fixture 必须有开关且默认关。采纳。
- **Arden**：同意 C；记录 B 为"下一次演化候选"。

## Decision

Accepted → [ADR-0003](../decisions/ADR-0003-ow0-static-web-ui.md)
