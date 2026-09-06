# ISS-017: 帧节奏修复（据 ISS-015 结论）

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | feature / perf |
| Owner | rin/rue + ver/vic |
| Reviewer | Dee（p50/p95 数据验收） |
| Status | open |
| Depends on | ISS-015 |

## 需求 / 验收标准

- [ ] 按 ISS-015 归因结果修复（候选：为 DrawCube 加 BeginMode3D 内的批处理评估 / 禁用 vsync 与 DWM 交互实验 / 固定步长累加器进真人路径——**固定步长是 DEC-003 的自然延伸，smoke 已在用**）。
- [ ] 修复后重测：p95 帧时间 ≤ 16.7ms 的占比 ≥ 99%（开发机）。
- [ ] `--smoke` 仍 5 连跑逐字节一致（节奏修复不许破坏确定性）。
- [ ] Owner 实测："卡顿严重"主观感受消失，B4（拖窗回来不瞬移）可判定。

## Non-goals

Job system、多线程渲染（charter §4 禁令——36 盒轮不到它们）。
