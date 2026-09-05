# ADR-0004: 事件模型 v1.1（Run 协议与教学层扩展）

- Date: 2026-09-06
- Status: accepted
- Supersedes: 无（扩展 ADR-0001，不取代）
- Superseded By: （空）
- Refs: RFC-0004, RUN-0001

## Context

RUN-0001 协议引入 run/phase/logical_seq 重放模型与 mentor/learn/ambient 事件族。ADR-0001 的 v1 模型缺少这些概念，但"append-only、单一事实源、seq/ts 纪律"不可破坏。

## Decision

1. envelope 追加**可选**字段 `run_id / phase_id / logical_seq / process_mode`；`schema_v` 保持 1（超集兼容，历史事件无需迁移）。
2. 新增类型：`project.state_discrepancy`、`run.opened/checkpoint/blocked/resumed`、`mentor.question/response`、`learn.lookup/misconception/resolved`、`ambient.note`。
3. `ambient.note` 四条硬约束（**不可协商**）：
   - `anchor_to` 必填且必须指向真实 artifact（worklog / issue / pr / commit / activity id）；
   - 永不改变任何状态（不碰 issue/PR/gate/metric）；
   - Gate 与 Retrospective 视图默认过滤，Studio 提供 Show Ambient 开关；
   - 不得包含技术事实（不得虚构 bug/benchmark/结论；感受可以写，事实不行）。
4. 事件日志写入权收敛到 Orchestrator（RUN 内），避免并行写 seq 冲突。
5. 校验政策：`obslog check` 对未知 type 报 warning 不报 error（Dee 条件）。

## Alternatives

独立第二日志（破坏单一事实源）；推迟到 v2（RUN-0001 过程无 trace）。均拒绝，理由见 RFC-0004。

## Consequences

- 正面：RUN-0001 全过程可回放；历史零迁移。
- 负面：旧版 v1 schema 文件会拒绝新 type——reader 政策为"忽略未知 type"；schema 随 repo 更新。
