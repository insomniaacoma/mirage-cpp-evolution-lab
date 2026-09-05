# ADR-0001: Observatory 以 append-only JSONL 事件日志为唯一事实源

- Date: 2026-09-05
- Status: accepted
- Supersedes: 无
- Superseded By: （空）
- Refs: RFC-0001, ISS-008

## Context

charter P3 要求从第一天记录项目历史，供未来 Studio / Interaction / Architecture / Development / Time Machine / Replay 六类视图使用。M0 无工具、无 UI、无数据库。存储形式必须在"今天就能用"与"未来可被工具消费"之间取得平衡（详见 RFC-0001 的备选分析）。

## Decision

1. 项目历史以 **append-only JSONL 事件日志**存储于 `observatory/data/log/YYYY-MM-DD-<topic>.jsonl`。
2. 事件 envelope 固定为 `{seq, ts, type, actor, schema_v, refs, payload}`；`seq` 全局单调；envelope 由 `observatory/schema/event.schema.json`（schema_v 1）约束。
3. 日志是**唯一事实源**；所有视图（图/时间线/回放）是从事件重放的**只读投影**，禁止反写。
4. 历史行永不修改、永不删除；语义变化通过新事件表达；schema 演化走 ADR。
5. 崩溃安全约定：文件末尾被截断的最后一行在读取时忽略。
6. envelope 校验先于任何消费数据的 UI 存在（ISS-008 的验收条件，Dee 的否决条款）。

## Alternatives

- SQLite（拒绝：二进制、不可人工审读、对 M0 过重；未来可从 JSONL 导入）。
- 每实体一 JSON 文件（拒绝：跨文件一致性与时点切片不可行，append-only 缺失）。

## Consequences

**正面**：第一天即可记录；git diff/审读友好；崩溃安全；演化历史天然完整。
**负面**：查询需要工具（后置）；payload 级校验缺位期间（ISS-008 前）格式错误可能混入——已知的、被接受的债。
**留下的债**：手写 JSONL 的出错率（Mora 预言，疼痛点 P5）→ 由 ISS-008 偿还。
