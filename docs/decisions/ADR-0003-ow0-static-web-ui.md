# ADR-0003: OW0 采用静态 Web UI + Python snapshot generator

- Date: 2026-09-06
- Status: accepted
- Supersedes: 无
- Superseded By: （空）
- Refs: RFC-0003, ISS-011, ISS-012, RUN-0001
- **Autonomous Project Decision**: yes

## Context

见 RFC-0003。OW0 需要七视图 + 回放，零额外依赖，checkpoint 级更新节奏。

## Decision

1. UI 线：**纯静态 Web**（`observatory/view/index.html` + 手写 JS/SVG，零外部依赖，file:// 打开即用）。
2. 数据线：**`tools/obslog.py view`**（Python 标准库）把事件日志 + checkpoints 计算为投影，生成 `observatory/view/data.js`。
3. 投影逻辑只在 generator 侧实现（Ash 条件）；前端只渲染。
4. 每个数据块携带 `source: "repository" | "fixture"`；历史视图默认只显 repository，`Show Fixture` 开关打开才显示 fixture 内容（Dot 条件，§6 硬约束）。
5. 演进候选（未排期）：本地 server 热更新（方案 B），由真实压力触发。

## Alternatives

Dear ImGui（依赖重、开发慢）；C++ core + server（双运行时，M0 同期不现实）。

## Consequences

- 正面：OW0 与 M0 完全解耦，不互相阻塞；回放 = 纯函数，可测试。
- 负面：数据变化后需手动重跑 `obslog.py view`（接受：checkpoint 节奏）；无实时交互。
