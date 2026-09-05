# ISS-011: OW0 view 数据生成器（events → data.js 投影）

| 字段 | 值 |
|---|---|
| Milestone | OW0 |
| Type | feature（工具） |
| Owner | mora/milo |
| Reviewer | dee/dot |
| Status | in-progress（RUN-0001 · P2） |
| Depends on | ISS-008 |
| Refs | RFC-0003 / ADR-0003, RFC-0004 |

## 需求 / 验收标准

- [ ] `tools/obslog.py view`：读取全部日志 + checkpoints → 投影 → 生成 `observatory/view/data.js`（`window.OBS_DATA`）。
- [ ] 投影只在 generator 侧（ADR-0003 条款 3）：issues 看板状态、模块图、studio 各 checkpoint 状态、讨论、metrics 时间线、事件流。
- [ ] 每个数据块带 `source: "repository" | "fixture"`；无法从事件得知的信息（如成员实时编辑文件）只允许出现在 fixture 块。
- [ ] data.js 可提交（是投影不是事实源）。

## Non-goals

实时刷新、server、编辑器。
