# ISS-013: OW0 fixture 政策与标记实现

| 字段 | 值 |
|---|---|
| Milestone | OW0 |
| Type | process |
| Owner | mora/milo + dee/dot |
| Reviewer | dee/dot |
| Status | open（RUN-0001 · P3 处理） |
| Depends on | ISS-011, ISS-012 |

## 需求 / 验收标准

- [ ] §6 硬约束落地：fixture 数据全部集中在 `data.js` 的 `fixtures` 块，UI 端任何 fixture 渲染必须有视觉标记（如角标 FIXTURE）。
- [ ] `Show Fixture` 默认关；Gate/Retrospective 视图（Metrics、Process Lab）永不禁用过滤。
- [ ] Wave Review 时盘点：哪些视图用真实数据、哪些仍 fixture（进入 ISS-014 输入）。

## Non-goals

fixture 的自动化采集（OW1 候选）。
