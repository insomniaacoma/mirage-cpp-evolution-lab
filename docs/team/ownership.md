# Team Ownership

- Version: 1 · Date: 2026-09-05
- 每位成员的公开档案与私人笔记在 `docs/team/members/`。私人笔记是信息局部性（charter P6）的载体。

## 1. 花名册

| ID | 名字 | 角色 | Ownership 域 | Watchpoints（观察点，非机械 KPI） |
|---|---|---|---|---|
| `owner` | **你** | Project Owner + Learning Developer | 最终架构裁决权；🎓 标记任务的实施者 | 理解每一次架构变化 |
| `member:arden` | Arden | Lead Architect | **无模块（刻意克制）**——拥有的是 dependency direction、ADR 质量与流程守门 | 依赖方向、API 稳定性、技术债是否被记录 |
| `member:nico` | Nico | Gameplay Engineer | gameplay 代码、World Graph 玩法需求、游戏规则 | iteration speed、可玩性；警惕为洁癖牺牲迭代 |
| `member:vera` | Vera | Core / Systems Engineer | app shell / 主循环；（未来）memory、events、platform | lifetime、错误处理；警惕提前造 platform 层 |
| `member:rin` | Rin | Rendering Engineer | **渲染模块未出生**——M0 以顾问身份参与 RFC-0002 与 ISS-003/004 的 review | frame time、GPU/资源生命周期 |
| `member:kai` | Kai | Audio Engineer | **音频模块未出生**——守护 realtime 边界原则，等需求出生 | audio thread 不被 gameplay convenience 破坏 |
| `member:mora` | Mora | Tools / Editor Engineer | observatory 工具链（recorder / validator / 未来 viewer）、build 脚本 | 开发者可用性；工具 10 分钟能上手 |
| `member:dee` | Dee | QA / Performance Engineer | 验证、runbook、metrics 快照；**持有"不可验证则拒绝"的否决权** | 可验证性、回归、build 稳定 |

## 2. 决策权

1. **模块内**：模块 owner 在自己域内自由决策，不必请示。
2. **跨模块**：必须走公开流程（Issue → RFC → 讨论 → ADR）。想改别人的 API，开 RFC，不能假定对方同意。
3. **架构方向**：由 Owner 最终裁决（charter P5）。Arden 负责把选项、代价和分歧摆清楚。
4. **QA 否决**：功能正确但无法验证/复现的 PR，Dee 可以拒。

## 3. 模块注册表

当前为空——**第一个代码模块**在 ISS-003 落地时由 Vera 注册（`module.registered` 事件）。模块注册表随代码出生而增长，不预先规划。

| 模块 ID | Owner | 出生于 | 状态 |
|---|---|---|---|
| *(尚未注册)* | — | — | 第一个将注册的是 `app`（主程序，Vera） |

预期但未出生的域（**只有占位意义，不含任何技术设计**，出生条件 = charter P1 的真实压力）：rendering（Rin）、audio（Kai）、editor/tools（Mora）、core 子系统（Vera）。

## 4. 协作规则

- 所有跨人协作发生在公共载体上：Issue 评论、RFC 讨论、PR review、会议记录，并同步进 observatory 日志。
- 允许的边类型（Observatory Interaction Graph 用）：`review` / `rfc_discussion` / `dependency_request` / `conflict` / `block` / `pair_work`。
- 争论的胜负标准是论据与验证（原型、benchmark、测试），不是职级。

## 5. 观察点（Qualitative，非竞赛指标）

按 charter §17，指标用于观察趋势，不用于机械 KPI 竞赛。每位成员的 watchpoint 见花名册列；量化指标统一由 Dee 在每个 release 打 `metrics.snapshot`。
