# ISS-007: 出口门控与胜利状态

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | feature |
| Owner | Nico |
| Reviewer | Dee |
| Status | open |
| Depends on | ISS-006 |

## 背景

逃逸跑酷的闭环：有出口、有条件、有反馈。

## 需求 / 验收标准

- [ ] 出口房间在突变前**不可达**（被 WorldGraph 拓扑封死，不是看不见）。
- [ ] 突变后可达；玩家进入出口区域后显示清晰反馈（如全屏文字 `LAYER 01 — CLEARED`），随后可退出或停留。
- [ ] 到达出口这件事写入 observatory（QA 验收时核对事件流）。
- [ ] Dee 的验收方式：按 ISS-009 的 checklist 亲手跑通全流程一次。

## Non-goals

计分、计时、下一层加载（M0 到此为止）、死亡重试。

## Notes

- 「LAYER 01」的措辞是有意的：为将来的多层结构埋叙事伏笔，但**不建**任何 level 系统。
- 胜利反馈允许最简：清屏 + 文字即可，不许为此加 UI 框架。
