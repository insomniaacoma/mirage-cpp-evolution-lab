# RUN-0001 · Final Gate（P5 · Dee 宣布）

- main @ P5 · M0 Exit Criteria vs OW0 Wave Goal 逐条核对

## Game M0 DoD

| DoD 项 | 证据 | 结论 |
|---|---|---|
| 可构建、可运行的 playable prototype + build 文档 | clean build 9s / 0 err；`docs/build.md`；smoke PASS×5 逐字节一致 | ✅ |
| M0 Issues 状态闭环 | ISS-001..009 全部 done（010 retro 在 P6 闭环） | ✅ |
| 必要 RFC/ADR（含渲染库裁决） | ADR-0002（raylib, Autonomous）+ 0003/0004；RFC-0002/0003/0004 全 accepted | ✅ |
| 真实 PR review 记录 | REVIEW-0001（含 Request Changes 往返）/0002/0003/0004/0005，两轮修改真实发生 | ✅ |
| smoke test 通过 + metrics 采集 | 见下 metrics.snapshot；checklist 实测记录 | ✅ |
| M0 Retrospective | P6 执行（R-001） | ✅（时序上随后） |
| 真实使用 Milestone/Vertical Slice/Spike/RFC·ADR/PR Review | 全部有真实工件，无凑模式 | ✅ |

**诚实声明（deferrals）**：B1-B4/C3/D3 人肉手感与察觉率项无法在本环境自动化，Owner 未在场 → 列为交付验收项 + Kai retro 议题。M0 的"可玩"结论由确定性走位 + squeeze 探针 + 几何截图支撑。

## OW0 Wave Goal

| 项 | 证据 | 结论 |
|---|---|---|
| 可运行/可点击/可回放 | 浏览器实测：七视图渲染、checkpoint 切换、讨论逐步回放、timeline 游标 | ✅ |
| Studio/Architecture/Work/Timeline/Discussions/Metrics/Process Lab | 全部渲染验证（architecture 待更多模块，容器已就绪） | ✅ |
| 真实 event projection + fixture 标记 | data.js source 块标记；Show Fixture 默认关；Gate 视图永无 fixture | ✅ |
| Role Lens v1 / Code Evolution View | **部分**：member 侧栏（点 avatar）已有；完整 Role Lens 与 Code Evolution View 明确**未达** → 记 OW1 | ⚠️ deferred |
| checkpoint + worklog | P0/P1/P2 snapshots + 5 份 worklog + 3 条 mentoring + 4 条 lookup | ✅ |

**OW0 结论：Wave Goal 达成（Code Evolution View / 完整 Role Lens 显式延至 OW1，不掩盖）。**

## Gate 宣布

**Final Gate: PASSED** —— 允许 tag `v0.1`，随后执行 metrics.snapshot 与 milestone.completed。
Dee 附注：本 Gate 的绿灯范围 = 自动化可证部分；所有主观手感项的最终验收人是 Owner。
