# ISS-012: OW0 Web UI 七视图

| 字段 | 值 |
|---|---|
| Milestone | OW0 |
| Type | feature |
| Owner | mora/milo |
| Reviewer | ard/ash + dee/dot |
| Status | in-progress（RUN-0001 · P2） |
| Depends on | ISS-011 |
| Refs | ADR-0003, §6 |

## 需求 / 验收标准

- [ ] `observatory/view/index.html + app.js + app.css`，零外部依赖，file:// 打开即用。
- [ ] 视图：Studio（房间网格 + avatar + 状态 chip + 关系边 + 点击详情面板）、Architecture（模块图）、Work（issues 看板 + PR 状态）、Timeline（事件流 + checkpoint 游标）、Discussions（逐步回放）、Metrics（快照对比）、Process Lab（M0 vs OW0 双板 + Gate 状态）。
- [ ] `Show Fixture` 开关：默认关，历史视图只显 `source=repository`。
- [ ] 回放按 `logical_seq`，审计信息（ts）可见但不作排序键（D4）。

## Non-goals

完整 IDE、实时协作、动画演出。
