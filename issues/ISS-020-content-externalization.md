# ISS-020: 内容数据外置（WorldGraph 数据文件 + 加载器）

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | feature（跨模块契约，T2） |
| Owner | nic/nell |
| Reviewer | Arden（数据格式成为契约的第一次亮相） |
| Status | open |
| Depends on | ISS-015（不与 016/017 冲突可提前并行） |

## 背景

`build_demo_world()` 的硬编码数据（疼痛点 P3 的"第二个消费者"时刻）——M1 的三个视觉 Issue 会反复调房间数据，每次改颜色都要重编译是不可接受的迭代速度损耗。这就是 charter P1 说的"真实需求出现"。

## 需求 / 验收标准

- [ ] 数据格式：最简人可读（JSON 或自定义文本），只承载 M0 已有的字段（rooms/connections/mutation spec/房间主题色），**禁止**为未来设计字段。
- [ ] `assets/world01.dat` + `load_world(path) -> WorldGraph`；加载失败给出行号级错误。
- [ ] smoke 行为零变化（同一份数据搬迁，5 连跑逐字节一致 = 确定性回归）。
- [ ] `build_demo_world()` 删除或退化为测试 fixture。
- [ ] 格式文档一段话写进 `docs/`（数据格式自此成为跨模块契约：改动走 review）。

## Non-goals

编辑器（Tools 轨远期）、热重载（M2+）、通用资产系统。
