# Learning Hook — Room 身份与失效矩阵（RUN-0001 · P2 · guided）

> 来源：vic 的 WorldGraph 实现 + vera 的 socratic mentoring（全文见
> `observatory/mentoring/RUN-0001/P2/vic-vera.md`）。这是 Owner 🎓 ISS-005 的替代材料：
> 读它 → 关掉它 → 自己重写 `world_graph.h` → 对比。

## 当时面临的问题

图结构里每个房间都要被别的东西引用（门洞、玩家所在房、mutation 目标）。
引用用什么表示？这决定了 mutation 出现时会不会出事。

## 关键决策链（问题 → 尝试 → 卡住 → 证据 → 结论）

1. **index vs handle vs 指针**
   - vic 第一反应：`Room*` 直接指——立刻撞上 `std::vector` 重分配使指针失效。
   - 查证（lookup-001）：cppreference——vector 扩容使**全部**引用失效。
   - M0 落点：`RoomId = int`（下标即身份），配 append-only 不变量（房间永不删除）。
2. **"为什么不一开始就上 generation 计数？"**
   - vera 的反问：一个没有读者的机制，维护成本谁付？
   - 证据：M0 没有 `remove_room`，generation 没有消费者。协议 = **revisit 条件写进头文件**：删除语义出现那天，id→index 映射、find_room 生命周期、所有持有 RoomId 的副本一起重审。
3. **双重失效的正交性**（vic 自己发现的问题，本 hook 的核心）
   - `version()` 管"拓扑变了吗"（逻辑层，读者是投影重建）。
   - 借出规则管"指针还能用吗"（内存层，读者是持引用者）。
   - 两者从同一组 mutator 长出来，但**失败方式不同**：前者浪费（多重建一次），后者是 UB。
   - 自查作业（未完成，进 R-001 待办）：三个 mutator × 三种借出物（`Room*` / `rooms()&` / `connections()&`）画失效矩阵。

## 关键代码（world_graph.h 头部注释，全文见源文件）

```cpp
// M0 invariant: rooms are append-only and never removed at runtime, so a
// RoomId stays valid for the whole process lifetime and is never reused.
// ... When deletion semantics ever appear, revisit: id->index mapping,
// find_room() result lifetime, and every RoomId copy held by scene/mutation code.
```

## 一个替代方案（被证据否决）

`struct RoomHandle { int id; int generation; }` + 全局 generation 表——正确但 M0 无消费者。
**rejected by evidence**：投机泛化，upgrade 路径已由疼痛点 P2 登记。

## Review 问题（自测）

1. 为什么 `connect()` 的 no-op 分支不能 `++version_`？（提示：version 的读者会做什么多余的事）
2. `find_room()` 返回 `const Room*`，如果改成返回 `std::optional<Room>`（值语义），调用方代码会多写什么？换来什么？
3. C++20 的 aggregates 为什么没有自动 `operator==`？（vic 真实踩坑，cppreference: default comparisons）

## 建议 Owner 亲手做的小练习

不看 git 历史，从空文件重写 `world_graph.h`：先写下你的失效矩阵，再写 API。
写完后 diff `git show 34620a3:src/world_graph.h`，重点看你的不变量注释覆盖了 Vic 没覆盖的场景没有。

## 术语卡（guided）

- **append-only 不变量**：数据只增不改的承诺；它让"下标即身份"变得安全。
- **失效（invalidation）**：容器结构变化使指向其内部的引用/指针/迭代器不可再用。
- **generation / handle**：id + 版本号的组合，用来检测"这个 id 指向的旧对象已被替换"。
