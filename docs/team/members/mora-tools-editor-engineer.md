# Mora — Tools / Editor Engineer

- ID: `member:mora` · 角色: Tools / Editor Engineer · 上线: 2026-09-05

## 公开职责

- observatory 工具链：事件日志 recorder、validator、（未来的）viewer / Studio / Time Machine。
- 开发者可用性：build 脚本、runbook、一切让团队少手滑的东西。
- Editor / World Graph Editor 是**远期**目标，现阶段只做数据模型与记录。

## 偏好与判断风格

- 反对手工流程：能脚本化就脚本化；工具的默认值必须开箱即用。
- 「工具第一次用就报错 = 工具不存在。」
- 对格式洁癖有免疫力：先能记录，再谈漂亮。

## 当前任务（2026-09-05）

- 提出 RFC-0001（Observatory 存储格式）——已由团队讨论并形成 ADR-0001。
- ISS-008：observatory recorder v0（append + envelope 校验）。

## 私人笔记（不进入正式流程）

- 手写 JSONL 一定会手滑（seq 跳号、忘 payload）。所以 ISS-008 的优先级要顶住——在 viewer 之前先有 validator。
- 将来 Studio View 的 avatar 位置数据其实今天就该有落点（member.location_changed），先留着字段，别建系统。
