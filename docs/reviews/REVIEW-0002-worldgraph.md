# REVIEW-0002 · PR: feature/m0-worldgraph → main（ISS-005 数据结构切片）

- Reviewer: Arden（架构/API）· Nico（玩法消费方）· Vera（pod senior，另见 mentoring 文件）
- 分支: feature/m0-worldgraph · commits: 34620a3, 8812ef1
- 测试证据：test_worldgraph 6/6 通过（含故意注入失败验证 CHECK 真的会失败）。

## Arden

- [x] Correctness — 6 测试 + 幂等语义 + 失败路径验证
- [x] API — 与跨分支契约逐字匹配；`connect(a,a)` no-op 的选择写明了理由（无错误通道）
- [x] Ownership — append-only 不变量写在头文件最前；id 单调分配与 `rooms_.size()` 的耦合**显式标注 revisit 条件**
- [x] Lifetime — 借出规则三段注释（version 救不了内存失效这一条写得很诚实）
- [x] Thread safety — 单线程 M0，N/A（已隐含：无任何同步原语）
- [x] Dependency direction — 零依赖（连 raylib 都没有），方向干净
- [x] Tests — version 不变式被测试固定
- [x] Maintainability — 头文件注释是"约束文档"而不是"这行做什么"，符合 AGENTS.md
- 观察（不阻塞）：id 复用风险 = `add_room` 用 `rooms_.size()` 推 next id。M0 接受（不变量 + revisit 条件已注明）；登记为疼痛点 P2 伴生观察项，`remove_room` 出现之日即是本条触发之日。

**Verdict: Approve**

## Nico（消费方意见）

mutation 只需要 `connect/disconnect/connected`，齐了。`build_demo_world()` 放 main.cpp（玩法数据），不放这个文件——正确。**Approve**。

## Vera（senior）

代码自有主张（借出规则、no-op 语义）且都有论证；C4819（中文注释 em-dash 触发 MSVC 936 代码页误读）的处理——**代码文件纯 ASCII**——是本仓库 Windows 工具链下该固化的纪律，将记入 AGENTS.md 候选条款。mentoring 问题质量见 vic-vera.md。

**Verdict: Approve — merge。**
