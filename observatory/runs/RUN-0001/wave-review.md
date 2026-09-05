# OW0 Wave Review（与 Game Retro 分开 · P6）

- Chair: Mora · 输入：docs/qa/OW0-checklist.md + 浏览器验证记录

## 哪些 UI 用真实数据 / 哪些仍 fixture

| 视图 | 数据源 | 状态 |
|---|---|---|
| Work | repository（issue 事件重放） | ✅ 真实（14 卡片与 INDEX 一致） |
| Timeline | repository（logical_seq 重放） | ✅ 真实 |
| Discussions | repository（D-0001..0004） | ✅ 真实（分歧/条件原样保留） |
| Metrics | repository（metrics.snapshot） | ✅ 真实 |
| Process Lab | repository + checkpoints | ✅ 真实 |
| Studio | repository（onboarded 默认 + snapshot 叠加） | ✅ 真实（fixture 只用于"实时编辑文件"徽标） |
| Architecture | repository（module.registered） | ✅ 真实（2 模块，图很稀疏——容器等模块出生） |

**fixture 总量：1 块**（studio_live），有标记、默认关、Gate 视图物理隔离。

## event model 哪里出现压力

1. **测试结果没有事件类型**：测试跑了几次、全绿与否只能进 snapshot 和 checklist，重放时间线看不到"测试失败→修复"的抖动。→ v1.2 候选：`test.run` 事件。
2. **commit.recorded 全靠手抄**：Orchestrator 忘记/补记都会发生（本轮 seq 67 的 commit 实为 P0 时期）。→ OW1 自动化采集第一优先。
3. **logical_seq 在跨 phase 补记时容易错位**（本人就错位过一次，靠校验器抓住）→ 事件写入权收敛到 Orchestrator 是对的，工具化 add 是解。
4. **ambient 三条全部被用掉但很克制**（0–3/人/phase 的上限没有顶到）——宁少勿多的方向正确。

## replay 是否可信

- 事件层：seq 全局单调、envelope 校验、append-only 纪律未破 → **高可信**。
- 快照层：P1 快照缺 location 字段导致过一次投影 bug（已修 generator 侧默认回填）→ 教训：**快照也是要被投影的数据**，字段契约要跟 schema 一样严肃。
- UI 层：浏览器验证发现的 4 个 bug 修复前后的 commit 都在历史里——修复过程本身可回放，这是设计使然而非巧合。

## 哪些 agent interaction 可真实投影 / 哪些仍是模拟

- 真实投影：3 个 sub-agent pod（vic/milo/nell）的 worklog、lookup、mentoring 文件、commit author 链。
- 模拟边界：ard/ash、nic/nell（nell 例外——他是真 sub-agent）、rin/rue、dee/dot 的部分产出由 Orchestrator 按角色边界模拟；**所有**代码与测试结果均为真实执行产物，角色只是署名。
- 结论：interaction 图的边是真实的（讨论、review、mentoring 文件都在），但"角色人格"部分是叙述层——Studio 视图如实呈现状态机，不假装更多。

## OW1 最值得自动化的采集源（优先级）

1. git commit → `commit.recorded`（hook）
2. 构建/测试结果 → `test.run`（v1.2 schema 提案）
3. Issue 文件状态 → `issue.status_changed` 对账器
4. metrics 自动采集脚本（LOC/构建时间/exe 体积）

## Code Evolution View / Role Lens

- 未达 OW0 目标，**显式延至 OW1**（Final Gate 已记录，不掩盖）。数据基础（checkpoint + commit 事件）已具备。
