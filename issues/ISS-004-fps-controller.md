# ISS-004: 第一人称控制器 v0

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | feature |
| Owner | Nico |
| Reviewer | Vera |
| Status | open |
| Depends on | ISS-003 |

## 背景

玩家要能在世界里走动。M0 只需要走 + 看。

## 需求 / 验收标准

- [ ] WASD 相对朝向移动，鼠标控制 yaw/pitch（pitch clamp ±89°）。
- [ ] 速度与 dt 缩放，帧率无关；准星可选。
- [ ] 碰撞最低要求：不穿墙——用「轴对齐墙体段 + 按轴分别解算」即可，房间数据来自 ISS-005 的 WorldGraph（若 005 未就绪，可先用临时方块自测，合并前接上）。
- [ ] 手感调过：走速、鼠标灵敏度有明确常量（硬编码合法）。
- [ ] 稳定 60 fps（Rin 抽查）。

## Non-goals

跳、蹲、加速跑、head bob、任何 parkour（charter §4：动作随玩法需求逐个出生）。

## Notes

- Nico：允许丑。允许每帧 rebuild 临时数组。这套代码的宿命就是被重写，别为它立抽象。
- Vera review 关注：输入状态谁拥有、mouse capture 的释放路径。
