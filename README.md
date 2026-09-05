# MIRAGE Cpp Evolution Lab

> **MIRAGE//AFTERIMAGE** — 第一人称 3D 梦核探索 / 时变迷宫 / 逃逸跑酷游戏。
> 本仓库同时是一场软件工程实验：我们要**观察**一个 C++ 工程如何从一个小原型演化成大型系统，而不是一次性设计出最终架构。

## 这是什么

双重目标：

1. **做出一个真正可玩的游戏**：探索陌生空间 → 察觉异常 → 学习规律 → World Graph 改写 → Collapse → 跑酷逃逸 → 下一层。
2. **首要目标**：让 Owner（你）能观察并解释这个 C++ 系统的每一次重要演化——抽象**为什么**产生、**何时**产生、代价是什么。

当"快速写出漂亮架构"与"让演化过程可见、可解释"冲突时，优先后者。见 [docs/charter.md](docs/charter.md)。

## 当前状态（2026-09-05 · Day 1）

| 项 | 值 |
|---|---|
| 当前里程碑 | **M0** — 极小可玩 3D prototype（未开始，见 [docs/milestones/M0.md](docs/milestones/M0.md)） |
| Issues | 10 个 open（ISS-001 ~ ISS-010，见 [issues/INDEX.md](issues/INDEX.md)） |
| RFC | RFC-0001 已接受（Observatory 事件日志）；RFC-0002（窗口/渲染方案）待 Owner 裁决 |
| ADR | ADR-0001 已接受 |
| 代码 | 尚无 — 将由 ISS-001 / ISS-003 创建 |
| Observatory | 数据模型 v1 + 第一天种子事件日志已就绪（[observatory/](observatory/README.md)） |
| Git | 尚未 init — 这是 ISS-001 的一部分，留给 Owner 亲手做 |

## 仓库地图

```
├── AGENTS.md                # Agent 团队工作守则（每个会话开始前必读）
├── docs/
│   ├── charter.md           # 项目 Charter：目标、原则、阶段禁令
│   ├── team/                # ownership 表 + 每位成员的档案与私人笔记
│   ├── process/             # 开发流程 + Issue/RFC/ADR/PR 模板
│   ├── milestones/M0.md     # Milestone 0
│   ├── rfcs/                # RFC 文档
│   ├── decisions/           # ADR 文档（只增不删，用 supersede 演化）
│   └── retrospectives/      # 每个 milestone 结束后的复盘
├── issues/                  # Issue 文件（一天一文件库，INDEX 为入口）
├── src/                     # 游戏代码（M0 时保持极简、扁平）
├── assets/                  # 资产（M0 不需要）
├── tools/                   # 开发工具（observatory recorder 等）
└── observatory/
    ├── README.md            # Observatory 章程
    ├── schema/              # 数据模型 + 事件 JSON Schema
    └── data/log/            # append-only 事件日志（项目历史的唯一事实源）
```

## 团队

7 名 Agent 工程师 + 你（Owner）：

| 成员 | 角色 |
|---|---|
| Arden | Lead Architect |
| Nico | Gameplay Engineer |
| Vera | Core / Systems Engineer |
| Rin | Rendering Engineer |
| Kai | Audio Engineer |
| Mora | Tools / Editor Engineer |
| Dee | QA / Performance Engineer |

分工、决策权与冲突规则见 [docs/team/ownership.md](docs/team/ownership.md)。每人有公开档案与私人笔记（`docs/team/members/`）——成员之间**允许真实存在分歧**，分歧会走公开流程解决，而不是被静默抹平。

## 工作方式（30 秒版）

1. 所有工作围绕 Issue；流程深度与风险成比例（[docs/process/process.md](docs/process/process.md)）。
2. 跨模块 API、新依赖、新抽象 → 必须走 Issue → RFC → 讨论 → ADR，不允许静默引入。
3. 每个 PR 都被真正 review，Reviewer 必须按 checklist 给出结论；**Merge 不是默认结果**。
4. 一切状态变化写入 `observatory/data/log/`（append-only JSONL）。**没有记录 = 没有发生。**
5. 标 🎓 的问题留给 Owner 亲手实现（ownership / lifetime / API 设计 / 数据结构 / 并发 / 架构边界），这是实验目的的一部分。

## 给 Owner 的下一步

1. 通读 [docs/charter.md](docs/charter.md)（约 10 分钟），它是整个实验的宪法。
2. 🎓 完成 [ISS-001](issues/ISS-001-repo-bootstrap.md)：git init、工具链选型、CMake 骨架。
3. 裁决 RFC-0002（窗口/渲染方案，团队推荐 raylib，最终由你决定，见 [ISS-002](issues/ISS-002-rfc-window-render-choice.md)）。
4. 之后每次会话：指定一个成员身份继续某个 Issue；会话结束前确认 observatory 日志已更新。

## 会话推进话术示例

- 「以 Nico 的身份继续 ISS-004」
- 「召开 M0 retrospective（ISS-010）」
- 「把今天的进展记录进 observatory」
- 「我想亲手实现 ISS-005 的 WorldGraph，让团队给我需求并 review」

## 文档状态图例

- Issue：`open / in-progress / in-review / blocked / done`
- RFC：`draft / review / accepted / rejected / superseded`
- ADR：`proposed / accepted / superseded`
- PR：`open / changes-requested / merged / rejected`
