# ISS-021: M1 Gate + v0.2 + R-002

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | process / qa |
| Owner | dee/dot |
| Reviewer | 全员 |
| Status | open |
| Depends on | ISS-015 ~ ISS-020 |

## 需求 / 验收标准

- [ ] 中间 Gate（视觉/音频/数据三线首次合流后）：构建 + smoke 确定性回归 + perf 基线对比。
- [ ] Owner 人肉验收（M1 DoD 第 1 条）：B4/C1–C5/D1–D4 全部可判定并通过。
- [ ] `docs/qa/M1-perf-baseline.md` 收尾（前后对比）。
- [ ] tag `v0.2` + `metrics.snapshot`。
- [ ] R-002 复盘：疼痛点对账（含音频 realtime 边界是否被考验、数据格式是否成为真实契约）。

## Non-goals

在 Gate 上做 M2 选型。
