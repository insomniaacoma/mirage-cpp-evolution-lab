# 开发流程

- Version: 1 · Date: 2026-09-05
- 模板见 [templates.md](templates.md)。

## 1. 流程总览（重要功能的默认路径）

```
Issue → Requirement Discussion → RFC → Owner Review → Design Discussion
      → ADR → Implementation Plan → Development Branch → Pull Request
      → Code Review → CI / Test / Benchmark → Merge → Release → Retrospective
```

## 2. 风险分级（charter P2：流程 ∝ 风险）

| 级别 | 典型修改 | 必需产物 | 明确不需要 |
|---|---|---|---|
| **T0 trivial** | typo、注释、.gitignore、文案 | 直接 commit + observatory 事件 | Issue、RFC |
| **T1 normal** | 单模块内新功能、bug fix、工具改进 | Issue → branch → PR → review | RFC、ADR |
| **T2 architecture-touching** | 跨模块 API、新模块、新第三方依赖、数据结构/线程边界、目录分层 | 完整链路（Issue → RFC → ADR → PR） | — |

判断口诀：**改了"别人怎么调用你" → T2**。拿不准时问 Arden；Arden 拿不准就升 T2。
常见 T2 例子：引入渲染/音频库（RFC-0002）、WorldGraph 的 mutation 接口被第二个系统消费、`main.cpp` 拆分出第一个模块。

## 3. Artifact 规范

### Issue
- 文件：`issues/ISS-<NNN>-<slug>.md`，登记进 `issues/INDEX.md`。
- 必填字段：id、标题、milestone、type、owner、reviewer、状态、依赖。
- 正文：背景 / 需求与验收标准 / Non-goals / Refs。🎓 标记表示 Owner 亲手实现。

### RFC
- 文件：`docs/rfcs/RFC-<NNNN>-<slug>.md`。
- 必须包含（顺序固定）：Context / Problem / Constraints / Proposal / Alternative A / Alternative B / Trade-offs / Migration / Risks / Open Questions / Reviewers / Decision。
- 讨论记录**必须**保留分歧（disagreement / question / rejection / compromise），不许只写"大家同意了"。

### ADR
- 文件：`docs/decisions/ADR-<NNNN>-<slug>.md`。
- 必须包含：Date / Context / Decision / Alternatives / Consequences / Status / Supersedes / Superseded By。
- **只增不删**。被取代的 ADR 保持原样，加 `Superseded By: ADR-XXXX`。

### Pull Request
- 描述按模板；Reviewer 按 checklist 分项给结论（Approve / Request Changes / Comment）。
- 不允许只写 "Looks good"。Merge 不是默认结果。
- 分支命名：`iss-<id>-<slug>`；commit 前缀：`feat:` / `fix:` / `docs:` / `infra:` / `obs:`。

### Retrospective
- 每个 milestone 结束后一次；文件 `docs/retrospectives/R-<NNN>-<milestone>.md`。
- 必答：哪些代码开始出现压力？哪些设计暂时仍然足够？下一阶段真正值得增加什么（以及为什么是现在）？

## 4. 状态词汇

- Issue: `open / in-progress / in-review / blocked / done`
- RFC: `draft / review / accepted / rejected / superseded`
- ADR: `proposed / accepted / superseded`
- PR: `open / changes-requested / merged / rejected`

## 5. Observatory 记录义务

每次状态变化都要在 `observatory/data/log/` 追加事件（append-only）。责任表：

| 事件 | 谁负责写 | 时机 |
|---|---|---|
| `issue.created` / `issue.status_changed` | Issue filer / 状态推动者 | 变化发生时 |
| `rfc.opened` / `rfc.decided` | RFC 作者 / Arden | 开启与裁决时 |
| `discussion.opened/turn/closed` | 主持人 | 每一轮讨论 |
| `adr.accepted` / `adr.superseded` | Arden | ADR 定稿时 |
| `pr.opened` / `pr.reviewed` / `pr.merged` / `pr.rejected` | 作者 / Reviewer / Merger | 各自环节 |
| `commit.recorded` | commit 作者 | merge 到主线时 |
| `module.registered` / `module.dependency_changed` | 模块 owner | 模块出生 / 依赖变化时 |
| `member.task_changed` / `member.location_changed` | 成员本人 | Studio 状态变化 |
| `metrics.snapshot` | Dee | 每个 release tag |
| `release.tagged` / `milestone.completed` | Owner / Arden | 发布时 |

ISS-008（recorder 工具）落地前手工追加，格式见 `observatory/schema/data-model.md`。

## 6. 目录约定

- `issues/`、`docs/rfcs/`、`docs/decisions/`、`docs/retrospectives/`、`docs/milestones/`、`docs/qa/`（Dee 建立时创建）。
- 新增**顶层目录**视为 T2。
