# ADR-0005: 流程治理修订 —— 🎓 履行形态、双轨节奏、认知负担政策

- Date: 2026-09-06
- Status: accepted
- Supersedes: 无（修订 charter P5 与 process.md 的运行解释）
- Superseded By: （空）
- Refs: RUN-0001, SD-1, Owner review after v0.1

## Context

v0.1 交付后 Owner review 提出三点治理裁决：

1. **SD-1 追认并常设化**：RUN-0001 中"🎓 任务由 Agent 实现 + Learning Hook/Guess 模式替代"的临时安排被证明有效（ISS-001/005 的 Hook 已成为 Owner 的实际学习入口）。
2. **节奏双轨**：游戏本体开发与扩展遵循 milestone 模式；观测站采用更快节奏的 wave 模式。RUN-0001 事实上已双轨运行（M0 + OW0 并行、各自 Gate），现在追认为常设规则。
3. **认知负担政策**：Owner 是初次进行大型 C++ 项目开发，专有名词与英文材料构成实际认知负担。

## Decision

1. **🎓 履行形态（常设规则，写入 charter v1.1 P5）**：
   - 🎓 任务的默认履行形态仍是 **Owner 亲手实现**；
   - 但 **"Agent 实现 + Learning Hook（guided）+ Guess 模式重做素材"** 是合法履行形态——条件是：(a) Hook 必须包含"当时的问题/关键代码/替代方案/Review 问题/Owner 小练习"；(b) 该任务的 Issue 状态注记必须显式记录转让（如 SD-1）；(c) Owner 保留否决权：任何 🎓 转让可在事后被 Owner 撤销并要求重做。
2. **双轨节奏（写入 process.md）**：
   - Game 轨：milestone 模式（M0→M1→…），完整 DoD + 双 Gate + Retrospective；
   - Observatory 轨：wave 模式（OW0→OW1→…），Wave Goal + Wave Review，允许更小粒度、更高频率的交付与修 bug 直通（T0/T1 直接做，不必等 Gate）；
   - 两轨各自记账（issues 的 Milestone 字段区分），交叉依赖显式声明。
3. **认知负担政策（写入 charter v1.1 + 立即执行项）**：
   - 建立 [docs/glossary.md](glossary.md)：所有高频术语给出一句人话解释，文档首次出现术语时优先链接它；
   - 观测站内容本土化：讨论与日志**从现在起新增内容中文优先**（技术标识符保留英文）；存量英文讨论在观测站中提供中文摘要（generator 侧 `summary_zh` 字段，OW0.1 wave 交付）；
   - Agent 会话守则同步：面向 Owner 的正式报告默认中文。

## Alternatives

- 维持 🎓 严格亲手制（拒绝）：RUN-0001 证明 Hook 模式有效且 Owner 主动追认，僵化会拖慢实验节奏且违背 Owner 意愿。
- 观测站也走 milestone（拒绝）：观测站的价值在于快速反馈回路，重流程会杀死它。

## Consequences

- 正面：Owner 学习路径有了正式载体与质量标准；两轨节奏各得其所；认知负担被承认为一等设计约束。
- 负面/风险：Hook 质量参差会稀释 🎓 的学习价值 → 以 (a)(b)(c) 三条件 + Owner 否决权兜底；中文化增加 generator 维护面 → 限定为 `summary_zh` 一个字段。
