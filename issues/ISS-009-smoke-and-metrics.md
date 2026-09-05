# ISS-009: M0 smoke checklist + metrics snapshot v0.1

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | process / qa |
| Owner | Dee |
| Reviewer | Mora |
| Status | open |
| Depends on | ISS-003 ~ ISS-007 |

## 背景

M0 的功能线完成后，需要一次独立验收 + 第一份有代码的指标快照（v0.0.0-seed 只有文档指标）。

## 需求 / 验收标准

- [ ] `docs/qa/M0-smoke.md`：手动验收 checklist——构建、启动、走遍全部房间、触发突变、观察 cue、被封锁的来路、新路径、抵达出口、退出干净。每条有「预期/实际/通过」三栏，复现步骤写到新人可照做。
- [ ] Dee 亲手跑一遍 checklist（不许只信 Nico 的演示）。
- [ ] 发现问题 → 回建 Issue（不许口头修）。
- [ ] 打 tag `v0.1` 后记录 `metrics.snapshot`：code LOC、code files、modules、tests=0（诚实记录）、debug 构建时间、exe 体积、issues open/closed、rfcs、adrs。
- [ ] `release.tagged` + `milestone.completed` 事件（v0.1 tag 之后，ISS-010 之前）。

## Non-goals

自动化测试、CI、覆盖率（charter §17：coverage when practical——现在不 practical）。

## Notes

- Dee 的立场：M0 全手动是对的，但 checklist 必须在**功能合并前**就起草，验收时才不会被「能跑就行」绑架。
- 指标用途是趋势观察（charter §17），不做成员间竞赛。
