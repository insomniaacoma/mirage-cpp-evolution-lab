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
| [ISS-014](ISS-014-ow0-qa-and-wave-review.md) | OW0 QA + Wave Review 输入 | dee/dot | mora/milo | 011–013 | done | P6 完成 |

## M1 Wave（RUN-0002 · 看得清的世界 · Owner 裁决：视觉优先）

| ID | 标题 | Owner | Reviewer | 依赖 | Status | 备注 |
|---|---|---|---|---|---|---|
| [ISS-015](ISS-015-frame-blur-attribution.md) | 帧时间与模糊归因（测量） | rin/rue 🎓Hook | Dee | — | open | 证据先行 |
| [ISS-016](ISS-016-clarity-fix.md) | 清晰度修复 | rin/rue | **Owner** | 015 | open | 视觉判定权在 Owner |
| [ISS-017](ISS-017-frame-pacing-fix.md) | 帧节奏修复 | rin/rue + ver/vic | Dee | 015 | open | 不破坏确定性 |
| [ISS-018](ISS-018-room-identity-cue.md) | 房间差异化 + cue 增强 | rin/rue + nic/nell | **Owner** | 016 | open | 察觉率判定→M2 输入 |
| [ISS-019](ISS-019-audio-module-birth.md) | RFC-0005 + 音频模块出生 | kai/kit 🎓Hook | Vera | — | open | realtime 红线从第一行 |
| [ISS-020](ISS-020-content-externalization.md) | 内容数据外置 | nic/nell | Arden | 015 | open | 数据格式成为契约 |
| [ISS-021](ISS-021-m1-gate.md) | M1 Gate + v0.2 + R-002 | dee/dot | 全员 | 015–020 | open | |
