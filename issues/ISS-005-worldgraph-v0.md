# ISS-005: WorldGraph v0 + 房间 + 场景投影 — 🎓 Owner 亲手实现

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | feature（核心数据结构，T2 级学习任务） |
| Owner | **owner** 🎓（Nico 提供玩法需求，Arden 提供设计讨论） |
| Reviewer | Arden + Nico（双 review） |
| Status | open |
| Depends on | ISS-003 |

## 背景

M0 的灵魂：房间之间的连接关系是**数据**（World Graph），玩家看到的场景是它的**投影**。这是将来一切 Mirage / Collapse / World Graph 重写机制的地基。正因如此，它属于 charter P5 点名留给 Owner 的任务类别（数据结构 + ownership + API 设计）。

## 需求 / 验收标准

- [ ] 定义 `WorldGraph`：rooms + connections（纯值类型，`std::vector` + id 引用）。数据结构由 Owner 设计，Nico 只提需求。
- [ ] 硬编码 4–6 个房间 + 连接（一个 `.cpp` 里的数据块，合法且正确）。
- [ ] `build_scene(graph) -> boxes`：从图生成可渲染的几何（墙、地板、门洞）。**单向投影**：改数据 → 重建几何。
- [ ] 门洞即「连接的存在/不存在」：无连接处是实墙。渲染走 raylib 即可（ISS-002 结果）。
- [ ] mutation 入口：一个能改写图的函数（具体形态 Owner 定，ISS-006 会消费它）。
- [ ] 设计讨论记录进 observatory（`discussion.*`），分歧保留。

## Non-goals

通用图库、序列化、编辑器、运行时增删节点类型、任何 ECS 味道的东西。

## Notes（Owner 的学习目标 — 这是本 Issue 的一半价值）

- **id 引用 vs 指针**：图节点互相引用时，为什么 id + vector 是更稳的起点（resize 失效、悬垂、序列化困难）。
- **ownership**：房间数据谁拥有？投影出来的几何谁拥有？改图时旧几何何时销毁？
- **单向数据流**：为什么 M0 刻意选择「改数据→全量重建投影」这条笨路（答案：它把"数据是真的，画面是假的"这个不变量焊死，等 Mirage 需要局部更新时再由真实压力推翻——见 Rin 的计时）。
