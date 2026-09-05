# RFC-0004: 事件模型 v1.1 —— Run 协议与教学层扩展

- Status: accepted
- Authors: Mora, Ash
- Reviewers: Arden, Dee
- Created: 2026-09-06（RUN-0001 · P1 · logical）
- Decision: ADR-0004

## Context

RUN-0001 协议要求事件可按 `run_id/phase_id/logical_seq` 重放，并新增 mentor/learn/ambient 三类事件（§4.3, §6.x）。ADR-0001 的 v1 模型没有这些概念。

## Problem

扩展事件模型，同时**不破坏** ADR-0001 的已接受语义：不改写历史事件、旧 reader 忽略未知 type、seq/ts 纪律不变。

## Proposal

1. envelope 增加**可选**字段：`run_id`（string）、`phase_id`（string，如 "P2"）、`logical_seq`（int，run 内单调）、`process_mode`（string）。
2. 新增事件类型（payload 要点）：
   - `project.state_discrepancy`：`discrepancy_id / expectation / evidence / adopted_baseline`
   - `run.opened / run.checkpoint / run.blocked / run.resumed`：run_id + 相关状态
   - `mentor.question / mentor.response`：`pair, junior, senior, response_type(socratic|pointer|pair|review|direct), direct_reason?`
   - `learn.lookup`：`member, unknown, found, credibility, adopted, timebox_steps`
   - `learn.misconception / learn.resolved`：`member, claim, rejected_by / resolution`
   - `ambient.note`：`subtype, anchor_to{type,id}, text` —— 四条硬约束（anchor 必填 / 不改状态 / Gate 默认过滤 / 无技术事实）
3. `schema_v` 保持 1：新字段全部可选，旧事件在 v1.1 校验下依然合法（超集兼容）。
4. `event.schema.json` 更新 enum + 可选字段；`data-model.md` 追加 v1.1 注册表节。

## Alternative A — 独立第二日志文件（run 事件单独存）

隔离性好，但破坏"单一事实源 + 全局 seq"模型，Time Machine 切片复杂化。不选。

## Alternative B — 等 v2 再统一升级

会迫使 RUN-0001 的关键过程（mentor/learn/ambient）不落日志，违背 D3。不选。

## Trade-offs

得到：单一事实源保留；向前兼容。
放弃：enum 校验对旧 reader 暂时失效（旧 v1 schema 文件会拒绝新 type）——缓解：reader 政策即"忽略未知 type"，校验器随 repo 更新。

## Migration

纯追加。历史 33 条事件不动。

## Risks

- ambient 事件被滥用为剧情生成器 → 硬约束写进 ADR + Gate 过滤 + Wave Review 抽查。
- logical_seq 与真实 ts 混淆 → 文档明确：UI 回放按 logical_seq，审计按 ts。

## Open Questions

- Code Evolution View 的事件粒度（per-commit vs per-checkpoint）→ OW0 Wave Review 裁定。

## Reviewers

- **Arden**：接受；要求 ADR 明写"四条 ambient 硬约束不可协商"。
- **Dee**：接受；`obslog check` 必须对新旧 type 一视同仁做 envelope 校验（未知 type 报 warning 不报 error）。

## Decision

Accepted → [ADR-0004](../decisions/ADR-0004-event-model-v1.1.md)
