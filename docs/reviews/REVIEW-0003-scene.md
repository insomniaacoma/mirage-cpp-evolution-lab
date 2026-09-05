# REVIEW-0003 · PR: feature/m0-scene → main（ISS-005 场景投影切片）

- Reviewer: Rin（渲染 watchpoint）· Vera（集成）
- commits: fe5fd00（+ 8bb2763 merge、02f770b 中误带的 ow0 骨架已拆分为独立 commit）
- 证据：`--smoke` 输出 `scene solids=36 decor=5 graph_version=9`，与手工几何推演完全一致（8 个门洞边 × 3 盒 + 12 个无门边 × 1；5 地板；9 次拓扑变更）。

## Rin

- [x] Correctness — 门洞 carving 的区间减法 + lintel；门宽不足的共享边判为无门（诚实退化，注释写明）
- [x] ADR-0002 调用面 — raylib 只在 scene.h/scene.cpp/main.cpp ✓
- [x] Performance — **version() 守卫的按需重建**（每帧重建的幼稚方案被 review 条款否决后落地）；M0 规模下单次重建 ~40 盒，成本可忽略；每帧 0 分配（重建只在拓扑变化时发生）
- [x] Ownership — Scene 由 main 拥有，solids 借给 player 做碰撞（Nell 的消费契约已在 scene.h 注明）
- [ ] Maintainability — 观察项（不阻塞）：`shared_edge` 对**不相邻**房间的 connection 静默产生"无门实墙"——这是 M0 合法 jank，但 mutation 分支接上 D-E 后如果数据写错房间坐标，症状（墙封死）会被误读为玩法。要求：ISS-006 的 mutation 设计说明里写一句"新增连接必须保证房间几何相邻"。
- 债登记：整体重建在 Mirage 频繁局部变化时不可持续 = 疼痛点 P2，已在 M0.md 预登记，Rin 计时开始。

**Verdict: Approve — merge（已合并至 main，本工件补记）。**
