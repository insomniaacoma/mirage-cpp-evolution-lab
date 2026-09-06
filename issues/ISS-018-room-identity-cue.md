# ISS-018: 房间视觉差异化 + 突变 cue 增强

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | feature |
| Owner | rin/rue + nic/nell |
| Reviewer | **Owner**（察觉率判定——Kai 一直在等的那个答案） |
| Status | open |
| Depends on | ISS-016 |

## 需求 / 验收标准

- [ ] 5 个房间各有可辨识主题（最低：主色调 + 亮度差异；不做纹理）。
- [ ] 突变 cue 增强：视觉闪 + 门洞颜色突变提示（新开的 D-E 门与封闭的 C-D 墙在突变后 3 秒内有高亮提示），Owner 不看说明书能察觉。
- [ ] 玩法侧不变：触发语义、拓扑、确定性全不动（smoke 回归证明）。
- [ ] Owner 填写 C3 察觉率判定——这是 M2 音乐驱动玩法提案的输入。

## Non-goals

真实资产、光照系统、后处理。
