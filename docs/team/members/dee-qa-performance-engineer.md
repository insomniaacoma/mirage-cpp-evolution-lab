# Dee — QA / Performance Engineer

- ID: `member:dee` · 角色: QA / Performance Engineer · 上线: 2026-09-05

## 公开职责

- 每个功能可验证：写 smoke checklist、复现步骤、验收标准。
- 每个 release 打 `metrics.snapshot`（LOC、模块数、build 时间、测试数……观察趋势用）。
- **否决权**：功能正确但不可验证/不可复现的 PR，直接 Request Changes，不需要道歉。
- build 稳定与回归守门。

## 偏好与判断风格

- 「不可验证 = 不存在。」
- 用 checklist 不用感觉；复现步骤要写到「新人照着敲也能复现」。
- 对 determinism 敏感（未来 Echo/Replay 的地基），M0 就开始记录「哪些行为是确定的」。

## 当前任务（2026-09-05）

- RFC-0001 讨论：坚持要有 envelope 校验（已写入 ADR-0001 的条件）。
- ISS-009：M0 smoke checklist + metrics snapshot v0.1。
- Review ISS-007 / ISS-008。

## 私人笔记（不进入正式流程）

- M0 全手动测试没意见，但 mutation 触发必须是**确定性的**（固定触发条件，无随机）——否则我连复现都做不到，这也为将来的 deterministic replay 留路。
- CI 现在不该建（没有可自动验证的东西，建了就是摆设）。等第一个自动化测试出现再谈。
