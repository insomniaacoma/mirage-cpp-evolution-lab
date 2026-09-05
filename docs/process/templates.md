# 模板：Issue / RFC / ADR / PR Review

复制对应模板到目标文件后填写。方括号是占位符。

---

## Issue 模板

```markdown
# ISS-<NNN>: <标题>

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | feature / bug / infra / decision / process |
| Owner | <member:xxx 或 owner>（🎓 = Owner 亲手实现） |
| Reviewer | <member:xxx> |
| Status | open |
| Depends on | ISS-xxx |
| Refs | RFC-xxxx / ADR-xxxx |

## 背景
<为什么需要这个；它服务于当前 milestone 的哪个验收标准。>

## 需求 / 验收标准
- [ ] <可检查的条目>

## Non-goals
<这次明确不做什么。>

## Notes
<实现提示、风险、学习目标（🎓 任务写 Owner 该理解什么）。>
```

---

## RFC 模板

```markdown
# RFC-<NNNN>: <标题>

- Status: draft / review / accepted / rejected / superseded
- Authors: <...>
- Reviewers: <...>
- Created: <date>

## Context
<现状；什么需求/压力逼出了这份 RFC。>

## Problem
<要解决的具体问题，一句话能说清。>

## Constraints
<必须满足的硬约束：平台、时间、已有代码、团队原则（引 charter 条款）。>

## Proposal
<推荐方案。够具体，能据此写 ADR。>

## Alternative A
<方案 + 为什么没选它。>

## Alternative B
<方案 + 为什么没选它。>

## Trade-offs
<选了什么、放弃了什么、什么债被留下。>

## Migration
<从现状到方案的路径；需要动哪些文件/人。>

## Risks
<可能翻车的方式 + 早期信号。>

## Open Questions
<未决问题，逐条列。>

## Reviewers
<意见摘要，含分歧。>

## Decision
<链接 ADR；未决则写 pending。>
```

---

## ADR 模板

```markdown
# ADR-<NNNN>: <标题>

- Date: <date>
- Status: proposed / accepted / superseded
- Supersedes: ADR-xxxx / 无
- Superseded By: ADR-xxxx / （空）
- Refs: RFC-xxxx, ISS-xxx

## Context
<决策时的事实与压力。>

## Decision
<决定了什么，一句话 + 必要细节。>

## Alternatives
<被拒绝的选项与拒绝理由。>

## Consequences
<正面 / 负面 / 留下的债。>
```

---

## PR 描述与 Review 模板

PR 描述：

```markdown
## 摘要
<做了什么，为什么（链回 Issue）。>

## 变更点
- <文件/行为级列表>

## 测试 / 验证
<怎么证明它对；手动步骤写全。>

## 风险与迁移
<可能影响什么。>
```

Review 结论（每个Reviewer 必须逐项给结论，不接受 "Looks good"）：

```markdown
Verdict: Approve / Request Changes / Comment

- [ ] Correctness（逻辑对吗，边界呢）
- [ ] API（接口形状合理吗，会不会很快被迫改）
- [ ] Ownership（谁拥有数据/资源，清楚吗）
- [ ] Lifetime（有没有悬垂/泄漏/双释放的可能）
- [ ] Thread safety（现在或将来会被哪个线程碰）
- [ ] Dependency direction（依赖指向对吗，有没有反向泄漏）
- [ ] Tests（可验证吗；手动验证步骤完整吗）
- [ ] Performance（有量过吗，还是凭感觉）
- [ ] Maintainability（三个月后看得懂吗）
- [ ] Migration / Documentation（需要同步改的文档改了吗）
```
