# Mentoring — vic -> vera, RUN-0001 P2

- Date: 2026-09-06 · Status: resolved（vera socratic 回应如下；vic 的反驳窗口开到 P2 retro）

**Q (vic):** `version()` 这个计数放在 WorldGraph 内部、由每个 mutator 自己 `++version_`，这个归属对吗？还是它其实是 scene/投影层的关心点，WorldGraph 不该知道"有人要重建"这件事？

**Context:** ISS-005 契约（Ray 定的跨分支 API）把 `int version()` 放进了 WorldGraph，我是按契约实现的，没有异议权，所以这不是抱怨契约——我想搞清楚的是它**为什么**是对的设计，以及边界在哪。我现在的理解：version 回答的是"拓扑变了吗"，这是 WorldGraph 自己的状态，mutator 是唯一知道"刚刚发生真实变化"的地方（connect/disconnect 的 no-op 分支不加 1，我为此写了测试）；scene 的 `build_scene` 只是比较 before/after，不需要知道变化发生在哪。但我有两个没想透的地方：

1. `find_room()` 返回 `const Room*` 指向 `rooms_` 内部，`add_room()` 重分配会使它失效（cppreference：reallocation 使所有引用失效）。我在头文件里写了借出规则，但这只是文档不是机制。如果 scene 层贪方便在重建循环里持有这个指针跨一次 `add_room`，version 救不了它——version 只管拓扑，不管内存。这两个失效（逻辑层的 version、内存层的指针）都从同一个 mutator 集合长出来，我却只有一半有机制可依。
2. 如果将来 Mirage 需要"改了哪个房间"（局部重建投影，M0.md 疼痛点 P2 预测的就是这个），`int version` 立刻不够用，是不是意味着 version 从第一天就该是个更结构化的东西（比如 change stamp / per-room stamp），而 M0 的 int 只是它的退化形态？

**Expected response type:** socratic——请反问我，别直接给答案；我尤其想被挑战的是第 1 点（我可能把两个正交问题混在一起了）。

**S (vera) — response type: socratic:**
> 先回答你的问题用问题：你说"两个失效从同一个 mutator 集合长出来"——那我问你，version 的**读者**是谁？借出规则的**读者**又是谁？如果一个是"比较整数的人"，另一个是"持指针的人"，那它们失败的方式一样吗：一个失败表现为**做了多余的重建**（浪费），另一个失败表现为**读到了别人的房间**（错误）。哪种失败我们的测试能抓到？哪种只有 UB 报告器能抓到？
>
> 第二问：如果你现在就给每个房间挂 per-room stamp，请告诉我——M0 里**谁**会消费它？一个没有读者的机制，它的维护成本由谁付？反过来，等 Mirage 的"局部重建"真的出现时，从 `int version` 升级到 stamp，调用方（scene）需要改几处？升级成本和提前成本，哪个更可逆？
>
> 最后一问留给你自查：你的借出规则写的是"add_room 可能重分配"——那 `connect()/disconnect()` 会让 `Room*` 失效吗？把这三个 mutator 对三种借出物（`Room*`、`rooms()&`、`connections()&`）的失效矩阵画出来，看它是不是比你现在的文字规则更小、更可测。

**Resolution:**（vera 记录落地结论，vic 可反驳）
- M0 采纳：`int version` 留在 WorldGraph——它是"拓扑变化"的事实计数，mutator 是唯一诚实的变化声明者，归属正确。
- 第 1 点的正交性判断成立：version=逻辑层（读者是投影比较），借出规则=内存层（读者是持引用者）。M0 选择**文档 + review 纪律**而非机制（find_room 返回值 `Room` 的副本会让调用方写起来更啰嗦），可逆性由失效矩阵保障；一旦出现第一个真实悬垂，直接改 `find_room` 返回 `std::optional<Room>`（值语义），调用方改动可控。
- 第 2 点：不提前做 per-room stamp（无读者=投机泛化）。把它登记为疼痛点 P2 的**升级路径**：当 Mirage 需要局部重建时，version 的升级形态由那时的 RFC 决定。
- vic 的失效矩阵自查项记入 R-001 待办。

**Verified by:** `test_version_tracks_real_changes_only`（no-op 不加 version 已被测试固定）；借出规则纪律**无机制验证**（M0 无 sanitizer），此缺口由失效矩阵 + review 承担——诚实记录为已知债。
