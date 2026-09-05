# Mentoring — vic -> vera, RUN-0001 P2

- Date: 2026-09-05 · Status: open（等 Vera 回应）

**Q (vic):** `version()` 这个计数放在 WorldGraph 内部、由每个 mutator 自己 `++version_`，这个归属对吗？还是它其实是 scene/投影层的关心点，WorldGraph 不该知道"有人要重建"这件事？

**Context:** ISS-005 契约（Ray 定的跨分支 API）把 `int version()` 放进了 WorldGraph，我是按契约实现的，没有异议权，所以这不是抱怨契约——我想搞清楚的是它**为什么**是对的设计，以及边界在哪。我现在的理解：version 回答的是"拓扑变了吗"，这是 WorldGraph 自己的状态，mutator 是唯一知道"刚刚发生真实变化"的地方（connect/disconnect 的 no-op 分支不加 1，我为此写了测试）；scene 的 `build_scene` 只是比较 before/after，不需要知道变化发生在哪。但我有两个没想透的地方：

1. `find_room()` 返回 `const Room*` 指向 `rooms_` 内部，`add_room()` 重分配会使它失效（cppreference：reallocation 使所有引用失效）。我在头文件里写了借出规则，但这只是文档不是机制。如果 scene 层贪方便在重建循环里持有这个指针跨一次 `add_room`，version 救不了它——version 只管拓扑，不管内存。这两个失效（逻辑层的 version、内存层的指针）都从同一个 mutator 集合长出来，我却只有一半有机制可依。
2. 如果将来 Mirage 需要"改了哪个房间"（局部重建投影，M0.md 疼痛点 P2 预测的就是这个），`int version` 立刻不够用，是不是意味着 version 从第一天就该是个更结构化的东西（比如 change stamp / per-room stamp），而 M0 的 int 只是它的退化形态？

**Expected response type:** socratic——请反问我，别直接给答案；我尤其想被挑战的是第 1 点（我可能把两个正交问题混在一起了）。

**Resolution:** TODO (vera)

**Verified by:** TODO（vera 回应后由 vic 回填：结论是否被测试或后续 PR 验证）
