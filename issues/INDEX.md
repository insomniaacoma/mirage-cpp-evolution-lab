# Issue Index

| ID | 标题 | Owner | Reviewer | 依赖 | Status | 备注 |
|---|---|---|---|---|---|---|
| [ISS-001](ISS-001-repo-bootstrap.md) | Repository bootstrap：git、工具链、CMake 骨架 | owner 🎓→ver/vic | Dee | — | done | git 部分由 Owner 完成 2026-09-06；余项 RUN-0001 SD-1 |
| [ISS-002](ISS-002-rfc-window-render-choice.md) | RFC-0002：窗口/渲染方案 | Arden → owner 🎓裁决 | Rin, Vera | ISS-001 | done | raylib 5.5 · ADR-0002（Autonomous） |
| [ISS-003](ISS-003-app-shell.md) | App shell：窗口、主循环、退出 | Vera | Arden | 001, 002 | open | 注册 `module:app` |
| [ISS-004](ISS-004-fps-controller.md) | 第一人称控制器 v0 | Nico | Vera | 003 | open | |
| [ISS-005](ISS-005-worldgraph-v0.md) | WorldGraph v0 + 房间 + 场景投影 | owner 🎓 | Arden, Nico | 003 | open | 核心学习任务 |
| [ISS-006](ISS-006-world-mutation.md) | 脚本化世界突变 + 可察觉 cue | Nico | owner | 004, 005 | open | 确定性触发 |
| [ISS-007](ISS-007-exit-win-state.md) | 出口门控与胜利状态 | Nico | Dee | 006 | open | |
| [ISS-008](ISS-008-observatory-recorder.md) | Observatory recorder v0 | Mora | Dee | 001 | open | 工具，独立线 |
| [ISS-009](ISS-009-smoke-and-metrics.md) | M0 smoke checklist + metrics v0.1 | Dee | Mora | 003–007 | open | |
| [ISS-010](ISS-010-m0-retrospective.md) | M0 Retrospective（R-001） | Arden | 全员 | 全部 | open | tag v0.1 之后 |

Status 词汇：`open / in-progress / in-review / blocked / done`。每次状态变化追加 `issue.status_changed` 事件。

## OW0 Wave（RUN-0001 · ADR-0003）

| ID | 标题 | Owner | Reviewer | 依赖 | Status | 备注 |
|---|---|---|---|---|---|---|
| [ISS-011](ISS-011-ow0-view-generator.md) | OW0 view 数据生成器 | mora/milo | dee/dot | ISS-008 | in-progress | 投影只在 generator 侧 |
| [ISS-012](ISS-012-ow0-web-ui.md) | OW0 Web UI 七视图 | mora/milo | ard/ash | ISS-011 | in-progress | 零依赖，file:// |
| [ISS-013](ISS-013-ow0-fixture-policy.md) | OW0 fixture 政策 | mora/milo + dee/dot | dee/dot | 011, 012 | open | Show Fixture 默认关 |
| [ISS-014](ISS-014-ow0-qa-and-wave-review.md) | OW0 QA + Wave Review 输入 | dee/dot | mora/milo | 011–013 | open | P5 执行 |
