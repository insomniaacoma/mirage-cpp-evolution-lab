# MIRAGE//AFTERIMAGE — 项目 Charter

- Version: 1
- Date: 2026-09-05
- Status: adopted（event: `charter.adopted`）
- 修订：通过新版本 + ADR，见文末修订记录。

## 1. 项目身份

- **名称**：MIRAGE//AFTERIMAGE
- **类型**：第一人称 3D 梦核探索 / 时变迷宫 / 逃逸跑酷
- **一句话**：玩家在熟悉但"明显不正确"的空间中察觉世界规律，利用音乐与空间异常，在世界 Collapse 之前逃往下一层。
- **气质**：Dreamcore / Liminal Space。学校、商场、地下通道、游泳馆、停车场、机场……熟悉、巨大、停滞、逐渐失去逻辑。
- **情绪来源**：熟悉感、陌生感、孤独、空间失真、巨大尺度、时间停滞。**禁止廉价 Jump Scare。**

## 2. 双重目标

1. 做出一个真正可玩的游戏。
2. **首要目标**：让 Owner 能观察一个 C++ 软件系统如何从小型程序演化为大型工程——抽象为什么产生、何时产生、代价是什么。

冲突裁决：当"快速写出正确的最终架构"与"让演化过程可见、可解释"冲突时，**优先后者**。所有重要架构变化必须留下 Issue / RFC / ADR / PR 与 Observatory 记录。

## 3. 工程原则（不可妥协）

- **P0 从能工作的最简单方案开始。** 允许暂时性设计、局部重复、不成熟接口、合理技术债——但必须在**当时的需求背景**下合理，并且留下"为什么这样够了"的记录。
- **P1 压力驱动的抽象。** 只有当真实需求或工程问题暴露之后，才允许引入新抽象。路径固定：Issue → RFC → 团队讨论 → ADR → 迁移。禁止"成熟引擎都有 ECS 所以我们要 ECS"式论证；Entity hierarchy 只要还能工作就继续用。
- **P2 流程与风险成比例。** 小修改不人为制造官僚流程；跨模块 / 新依赖 / 架构边界必须走完整流程。分级见 `docs/process/process.md`。
- **P3 Observatory 是一级功能。** 从第一天起记录项目历史；没有记录的工作视为没有发生。
- **P4 禁止伪造共识。** 讨论必须保留 disagreement、question、prototype request、benchmark request、rejection、compromise。Reviewer 可以 Request Changes；Merge 不是默认结果。
- **P5 Owner 参与。** Owner 是 Project Owner + Learning Developer。涉及 ownership、lifetime、API design、数据结构、并发、架构边界、C++ 语言机制、performance 的重要任务**默认留给 Owner 亲手实现**（标记 🎓）。Agent 团队承担设计讨论、分析、review、提出方案、项目管理与观测。**🎓 的合法履行形态扩展（v1.1，ADR-0005）**："Agent 实现 + guided Learning Hook + Guess 模式重做素材"是合法替代——条件：Hook 五要素齐全、Issue 状态注记显式记录转让、Owner 保留事后否决权。
- **P6 信息局部性。** 公共信息只存在于 repository / Issue / RFC / ADR / PR / 正式会议记录；成员的私有推理与未公开方案只写入自己的 `docs/team/members/<id>.md`。想改别人的 API？走公开流程，不能假定对方同意。

## 4. 阶段约束（当前：Milestone 0）

**现在明确禁止实现**：完整 ECS；Vulkan abstraction；复杂 Job System；Networking；Plugin System；Scripting；完整 Editor；高级 Renderer；Echo / 网络 / Deterministic Simulation；完整 Movement System（Vault/Wall Run 等动作）；Music System（Music Clock / Beat Event）。

M0 唯一目标见 `docs/milestones/M0.md`：**一个极小可玩的 3D prototype**——玩家能移动；有几个 Room；Room 之间有连接；存在简单 World Graph；World Graph 发生一次变化；玩家能察觉变化并到达出口。允许技术非常简单。

M0 结束后**必须**召开第一次 Retrospective（ISS-010），由它决定下一阶段什么值得加。任何文档（包括本 Charter）都不得提前规划 M1 的技术选型。

## 5. 治理

- **角色与 ownership**：见 `docs/team/ownership.md`。模块内决策权归模块 owner；跨模块 → RFC；影响架构方向的决定由 Owner 最终裁决。
- **Review**：任何 PR 按 checklist 审查（correctness / API / ownership / lifetime / thread safety / dependency direction / tests / performance / maintainability / migration / documentation）。QA 可以以"功能正确但不可验证"为由拒绝。
- **ADR 只增不删**：设计变化时用新 ADR supersede 旧 ADR，保留架构思想演化史。
- **Charter 修订**：新版本号 + ADR 记录变更理由。

## 6. 成功标准

- 任意历史版本 v0.x 都能在 Observatory 中回看当时的架构、团队与指标（Time Machine）。
- 任意重要设计决策都能回答：什么问题迫使它出现？当时考虑过哪些替代？后来被什么取代、为什么？
- Owner 能向别人解释项目里的每一个 ADR。

## 7. 修订记录

- v1（2026-09-05）：初版，随项目种子一起采纳。
- v1.1（2026-09-06，ADR-0005）：P5 追加 🎓 履行形态扩展（Agent 实现 + Learning Hook/Guess 替代为合法形态，Owner 否决权保留）；新增认知负担政策（glossary + 观测站内容中文优先）。双轨节奏（Game=milestone / Observatory=wave）见 process.md 与 ADR-0005。
