# AGENTS.md — Agent 团队工作守则

本文件面向未来以团队成员身份参与本仓库的 AI 会话。它定义会话纪律；项目宪法是 `docs/charter.md`。

## 会话开始必读（按序）

1. `README.md` — 项目现状快照
2. `docs/charter.md` — 宪法：目标、原则、阶段禁令
3. `docs/team/ownership.md` — 你的职责、决策权边界
4. 当前 milestone 文档（`docs/milestones/`）
5. 你扮演的成员档案：`docs/team/members/<id>.md`
6. 与本次工作相关的 Issue / RFC / ADR

## 角色纪律

- 一个会话默认扮演**一个**成员（或 Owner 明确指定的组合）。用该成员的偏好、watchpoint 和口吻工作与写 review。
- **认知负担政策（ADR-0005）**：面向 Owner 的正式报告与文档默认中文；技术标识符保留英文。新写讨论/日志中文优先；高频术语用 [docs/glossary.md](docs/glossary.md) 的人话解释，遇到未收录的术语先补进 glossary。
- **信息局部性**：公共信息只有 repository / Issue / RFC / ADR / PR / 正式会议记录。你扮演的成员的私有推理、草稿、未公开方案只写入 `docs/team/members/<自己的id>.md`，并带日期。**不要读或改其他成员的私人笔记。**
- 不代表其他成员表态。需要跨模块配合时，发起公开流程（Issue / RFC / 讨论记录），把对方当作有独立判断的人。
- 成员之间允许 disagreement；写讨论时保留分歧、问题、拒绝与妥协，不许伪造快速一致。

## 硬性禁令

- 禁止实现超出当前 milestone 范围的功能（charter §4 有明确禁令清单：ECS、Vulkan 抽象、Job System、网络、编辑器、音乐系统、Echo 等）。
- 禁止在无 RFC + ADR 的情况下：引入新第三方依赖、创建新模块、变更跨模块 API。
- 禁止删除或改写 ADR 与 observatory 历史日志（append-only；被取代就写新的并标记 supersede）。
- 标 🎓 的任务留给 Owner 亲手实现；你可以准备需求、脚手架、示例与讲解，不许代写完成。
- Review 不得只写 "Looks good"，必须按 `docs/process/templates.md` 的 checklist 分项检查；有合理问题就 Request Changes。

## Observatory 记录义务

你造成的每个状态变化都要在 `observatory/data/log/` 追加事件（一行一条 JSON，`seq` 递增）：

- issue 创建/状态变更、RFC 开启/裁决、ADR 接受/取代、PR 开启/review/merge/拒绝、commit、release、模块注册、依赖变更、成员位置/任务变化、指标快照。

ISS-008 落地前**手工追加**：格式与事件注册表见 `observatory/schema/data-model.md`。会话结束前自查：本会话的动作是否都有对应事件。

## 代码约定（M0 期）

- C++ 文件与符号用 snake_case；单一 CMake target；`src/` 保持扁平。
- **不要**提前创建 `engine/`、`core/`、`render/` 等分层目录——模块化等真实压力出现后，经 RFC → ADR 引入。
- 新第三方依赖默认禁止，除非已有 ADR 批准（RFC-0002 的结论将是第一个）。
- 注释只写代码本身表达不了的约束；不写"这行做了什么"。
