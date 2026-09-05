# RFC-0001: Observatory 历史记录的存储格式

- Status: accepted
- Authors: Mora
- Reviewers: Arden, Dee
- Created: 2026-09-05
- Decision: ADR-0001

## Context

charter P3 规定 Observatory 是一级功能、从第一天开始记录项目历史。未来六个视图（Studio / Interaction Graph / Architecture Graph / Development Graph / Time Machine / Discussion Replay）全部从这份历史推导。今天就要写下第一批事件（团队建立、M0、Issue 等）。

## Problem

项目历史用什么形式存储，才能同时满足：今天就能开始记录（没有工具、没有 UI）、人能直接读、append-only 可审计、未来工具可解析。

## Constraints

- M0 无任何专用工具/服务器/数据库依赖。
- 记录不可篡改（append-only），这是"观察演化"的可信度基础。
- 必须能表达：事件（状态变化）、结构化讨论（逐轮、带立场）、指标快照（按版本）。
- Windows 文件系统友好；纯文本。

## Proposal

**Append-only JSONL 事件日志**，位于 `observatory/data/log/YYYY-MM-DD-<topic>.jsonl`。

- 一行 = 一个事件：`{seq, ts, type, actor, schema_v, refs, payload}`。
- `seq` 全局单调递增（跨文件）；读取时按 `seq` 排序。
- 视图（图、时间机器、回放）一律是**只读投影**，从重放事件推导；禁止反向写。
- 事件类型注册表与 envelope 规范见 `observatory/schema/data-model.md`；JSON Schema（envelope 级）见 `observatory/schema/event.schema.json`。

## Alternative A — SQLite 数据库

查询能力强、事务安全。但：二进制不可 diff/人工阅读；对"第一天"太重；备份/迁移复杂度提前到来。将来如果 Time Machine 需要复杂查询，可以**从 JSONL 导入** DB，日志仍是事实源。

## Alternative B — 每实体一个 JSON 文件（issue/ISS-005.json 等）

单文件好读好改。但：跨文件的一致性（一次 merge 动 5 个文件）与全局顺序难保证；Time Machine 需要的"按时点切片"几乎不可做；append-only 语义天然缺失。

## Trade-offs

- 得到：今天就能记录；git diff 友好；崩溃安全（一行一事件，尾行截断可忽略）；审计性完美。
- 放弃：即席查询要靠工具（ISS-008 提供 check，查询型工具后置）；schema 漂移风险 → 用 envelope JSON Schema + 注册表约束（Dee 的条件）。

## Migration

无（从零开始）。未来若引入 DB：写一次性 importer，JSONL 归档保留。

## Risks

- 手写事件拼错字段 → 缓解：ISS-008 validator + envelope schema（先于任何 viewer 落地，Dee 的验收条件）。
- 日志膨胀 → 十年内不是问题；届时再归档。

## Open Questions

- Studio 的成员位置/任务变化由谁、多频繁地记录？（倾向：成员自己在任务切换时记，v0 不自动化）
- metrics.snapshot 的采集用脚本还是手抄？（倾向 v0 手抄，ISS-009）

## Reviewers

- **Dee**：反对裸奔。要求 envelope 校验先于任何消费数据的 UI 存在；ISS-008 落地前允许手工但格式必须冻结。
- **Arden**：支持。追加约束：投影只读；`seq`/`ts` 纪律；schema 演化必须走 ADR，不允许悄悄改历史事件的含义。

## Decision

Accepted → [ADR-0001](../decisions/ADR-0001-append-only-event-log.md)。

## 讨论实录（结构化存档，同步于 observatory 日志 `discussion:D-0001`）

> **Mora** (proposal)：直接 JSONL，一行一事件。工具后置，今天先能记录。
> **Dee** (concern)：手写一定会 schema 漂移，三个月后没人能解析。没有校验我不接受任何工具消费这份数据。
> **Mora** (compromise)：折中——envelope 的 JSON Schema 今天冻结；payload 级校验和 validator 放进 ISS-008，作为它的验收标准。
> **Arden** (support + constraint)：可以，但要写死三条纪律：日志是唯一事实源；所有视图只读投影；改 schema 走 ADR、不改历史行。
> **Dee** (acceptance)：接受，条件是 validator 先于 Time Machine 消费数据落地，否则我 block。

最终立场分布：proposal ×1、concern ×1、compromise ×1、support ×1、acceptance ×1 —— 无全票通过，有条件接受。
