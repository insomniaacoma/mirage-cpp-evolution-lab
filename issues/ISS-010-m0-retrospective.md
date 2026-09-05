# ISS-010: M0 Retrospective（R-001）

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | process |
| Owner | Arden（主持） |
| Reviewer | 全员（每人必须留至少一条意见） |
| Status | open |
| Depends on | ISS-001 ~ ISS-009（tag v0.1 之后） |

## 背景

charter §4 / §18 的硬性要求：M0 结束后召开第一次 Retrospective，由它决定 M1 方向。**这是本实验核心机制（观察演化）的第一次完整运转。**

## 需求 / 验收标准

- [ ] 产出 `docs/retrospectives/R-001-M0.md`，必答三问：
  1. 哪些代码开始出现压力？（对照 M0.md 的疼痛点预测 P1–P5 逐条对账：真的疼吗？证据？）
  2. 哪些设计暂时仍然足够？（明确说「不用改」，防止无病呻吟式重构）
  3. 下一阶段真正值得增加什么？为什么是现在？（只定方向，不做选型）
- [ ] 每位成员从自己的 watchpoint 留意见（允许分歧，原样保留）。
- [ ] Owner 的意见单独成节：哪些地方想亲手做、想学什么。
- [ ] 由此生成 M1 的候选 Issue 列表（仅标题级，开工再展开）。
- [ ] observatory：`milestone.completed`（M0）+ retro 讨论的 `discussion.*` 事件。

## Non-goals

在 retro 上做技术选型（那是 M1 的 RFC 们的事）；为了「先进」而重写没疼过的代码（charter P1）。

## Notes

- Arden 主持时要防两个失败模式：全员和谐（说明有人没说真话）与全盘推翻（说明没对照疼痛点证据）。
- Kai 的「cue 察觉率」、Rin 的「重建盒子计时」、Vera 的「main.cpp 记账」都是预埋的证词，记得收集。
