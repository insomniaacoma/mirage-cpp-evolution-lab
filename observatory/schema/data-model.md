# Observatory 数据模型 v1

- Schema version: 1 · Date: 2026-09-05 · Status: accepted（ADR-0001）
- 演化规则：本模型变更走 ADR；历史日志永不改写；校验器必须容忍未知 `type`（向前兼容）。

## 1. 存储原则

- **Append-only JSONL**：`observatory/data/log/YYYY-MM-DD-<topic>.jsonl`，一行一个事件。
- `seq` 全局单调递增（跨文件）；读取按 `seq` 排序；允许跳号（报告但不视为致命）。
- 崩溃安全：文件末尾被截断的最后一行读取时忽略。
- 日志是唯一事实源；一切视图 = 只读投影（重放事件推导）。

## 2. Envelope

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `seq` | int | ✓ | 全局单调，从 1 开始 |
| `ts` | ISO8601 | ✓ | 事件发生时间（含时区偏移） |
| `type` | string | ✓ | 事件类型，见 §5 注册表 |
| `actor` | string | ✓ | `owner` / `member:<id>` / `system` |
| `schema_v` | int | ✓ | 当前为 `1` |
| `refs` | string[] | | 关联对象，格式 `issue:ISS-005` 等，见 §4 |
| `payload` | object | ✓ | 事件内容，必填字段见 §5 |

## 3. 实体模型

| 实体 | 关键字段 | 说明 |
|---|---|---|
| Member | `member_id, handle, role, location` | 团队成员；`location` 为 Studio 中的房间名（自由字符串，如 `gameplay-lab`、`design-room`） |
| Module | `module_id, owner, status` | 代码模块；出生时注册，死亡时 deprecated（不删除） |
| ModuleDependency | `from_module, to_module, kind` | 架构图与环检测的数据来源 |
| Issue | `issue_id, title, milestone, owner, reviewer, status` | 工作单元 |
| RFC | `rfc_id, title, status, path` | 设计提案 |
| Discussion | `discussion_id, topic, participants, turns[]` | 结构化讨论；turn = `{member, stance, text}`，stance ∈ `proposal/support/question/concern/rejection/compromise/acceptance/neutral` |
| ADR | `adr_id, title, status, supersedes, superseded_by` | 架构决策 |
| PR | `pr_id, branch, status, reviewers[]` | 评审记录（本仓库暂无 GitHub，PR 指文件/分支级评审） |
| Commit | `sha, subject` | 主线提交 |
| Release | `tag, notes` | 版本发布 |
| MetricSnapshot | `version, metrics{}` | Time Machine 的刻度 |
| Interaction | `type, participants[], weight` | 协作边：`review/rfc_discussion/dependency_request/conflict/block/pair_work` |

## 4. Ref 格式

`issue:ISS-005` · `rfc:RFC-0002` · `adr:ADR-0001` · `discussion:D-0001` · `pr:PR-0001` · `commit:<sha>` · `release:v0.1` · `milestone:M0` · `module:app` · `member:nico`

ID 规则：Discussion `D-0001+`；PR `PR-0001+`（首个真实 PR 产生时启用）；其余见 process.md。

## 5. 事件注册表

| type | payload 必填字段 | 供给的投影 |
|---|---|---|
| `project.created` | `name` | Dev Graph |
| `charter.adopted` / `charter.amended` | `path`, `version` | Dev Graph |
| `member.onboarded` | `member_id, handle, role, location` | Studio, Interaction |
| `member.location_changed` | `member_id, from, to` | Studio |
| `member.task_changed` | `member_id, task, refs` | Studio |
| `milestone.created` | `id, title, doc` | Dev Graph |
| `milestone.completed` | `id` | Dev Graph |
| `issue.created` | `issue_id, title, owner, reviewer, milestone` | Dev Graph, Studio |
| `issue.status_changed` | `issue_id, from, to` | Dev Graph |
| `rfc.opened` | `rfc_id, title, path` | Dev Graph |
| `rfc.decided` | `rfc_id, outcome, adr` | Dev Graph |
| `discussion.opened` | `discussion_id, topic, participants` | Replay, Interaction |
| `discussion.turn` | `discussion_id, member, stance, text` | Replay, Interaction |
| `discussion.closed` | `discussion_id, outcome, adr?` | Replay, Dev Graph |
| `adr.accepted` | `adr_id, title` | Dev Graph |
| `adr.superseded` | `adr_id, superseded_by` | Dev Graph |
| `pr.opened` | `pr_id, branch, author` | Dev Graph |
| `pr.reviewed` | `pr_id, reviewer, verdict` | Dev Graph, Interaction |
| `pr.merged` / `pr.rejected` | `pr_id` | Dev Graph |
| `commit.recorded` | `sha, subject` | Dev Graph, Time Machine |
| `release.tagged` | `tag` | Dev Graph, Time Machine |
| `module.registered` | `module_id, owner` | Architecture |
| `module.deprecated` | `module_id` | Architecture |
| `module.dependency_changed` | `from_module, to_module, added` | Architecture |
| `interaction.logged` | `type, participants` | Interaction |
| `metrics.snapshot` | `version, metrics` | Time Machine |

> v0 只在 envelope 级强制校验（见 `event.schema.json`）；上表 payload 约束由 ISS-008 的 validator 落实。

## 6. 投影定义（未来实现，勿提前建设）

- **Studio**：对每个 member 重放最后一条 `onboarded/location_changed/task_changed` → 当前状态。
- **Interaction Graph**：`discussion.turn`（同场讨论的参与者两两连边）+ `interaction.logged` + `pr.reviewed`（reviewer↔author）→ 加权图。
- **Architecture Graph**：重放 `module.*` 到目标时刻 → 模块依赖图 → 环/分层/fan-in-out 检测。
- **Development Graph**：`issue/rfc/adr/pr/commit/release` 实体 + `refs` 连边 → 功能链路。
- **Time Machine**：按 `release.tagged`/`metrics.snapshot` 切片，重放 `module.*`、`issue.*` 等到该时点 → 全视图快照。
- **Discussion Replay**：`discussion.*` 按序重放 turns + 关联 ADR。

## 7. v0 明确不做

viewer UI、自动 git 采集、数据库、payload 深度校验工具（ISS-008 只做 envelope + seq 检查）、成员状态自动同步。

## 8. v1.1 扩展（ADR-0004 · RUN-0001）

envelope 追加**可选**字段（schema_v 保持 1，超集兼容）：

| 字段 | 类型 | 说明 |
|---|---|---|
| `run_id` | string | `RUN-NNNN` |
| `phase_id` | string | `P0`–`P6` |
| `logical_seq` | int | run 内单调；UI 回放顺序 |
| `process_mode` | string | 执行模式注记（pair/solo/gate…） |

新增事件类型：

| type | payload 必填 | 供给 |
|---|---|---|
| `project.state_discrepancy` | `discrepancy_id, expectation, evidence, adopted_baseline` | Dev Graph, Run Report |
| `run.opened` | `run_id, track_goals, active_pairs` | Run View |
| `run.checkpoint` | `run_id, phase_id, snapshot_path` | Time Machine |
| `run.blocked` / `run.resumed` | `run_id, reason` | Run View |
| `mentor.question` | `pair, junior, senior, question` | Mentor Lens |
| `mentor.response` | `pair, junior, senior, response_type, direct_reason?` | Mentor Lens（direct 计入教学债务） |
| `learn.lookup` | `member, unknown, found, credibility, adopted, timebox_steps` | Junior Lens |
| `learn.misconception` | `member, claim, rejected_by` | Junior Lens（保留错误假设） |
| `learn.resolved` | `member, resolution` | Junior Lens |
| `ambient.note` | `subtype, anchor_to{type,id}, text` | Studio（默认过滤） |

`ambient.note` 四条硬约束见 ADR-0004 §Decision 3。
