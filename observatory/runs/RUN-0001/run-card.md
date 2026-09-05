# Run Card — RUN-0001

| 字段 | 值 |
|---|---|
| run_id | RUN-0001 |
| protocol | v3.1 |
| base_sha | `8e5f7ec2ff246c768bda46c991b0568c58fb5c8a` |
| context_budget | 启动时估算：充裕（新窗口）。每个 checkpoint 重估，进入红线即按 §8 停止并 handoff |
| stop_when | §8：Final Gate 通过；或 Gate 连续 2 次 Failed 无修复路径；或上下文红线；或全部剩余工作依赖 Owner |
| resume_from | null |
| pairing_mode | active |
| active_pairs | ard/ash · nic/nell · ver/vic · rin/rue（mora/milo 按需激活：OW0 工具链；kai/kit dormant） |
| difficulty_lens | guided |
| ambient_enabled | true |

## 轨道

| Track | 本轮目标 |
|---|---|
| Game M0 | 极小可玩 3D prototype（build → 移动 → WorldGraph → 一次可察觉突变 → 逃逸出口） |
| Observatory OW0 | 可运行 / 可点击 / 可回放的观测站 v1（Studio·Architecture·Work·Timeline·Discussions·Metrics·Process Lab；真实事件投影 + fixture 显式标记） |

## 执行模式声明（诚实记录）

- Orchestrator：不写业务代码、不做渲染设计、不裁决架构、不宣布测试通过。任务分解、调度、Phase 管理、事件记录、Gate 触发。
- Pod 执行方式：**ver/vic 与 mora/milo 使用真实 sub-agent（隔离上下文，git worktree 物理隔离）**；ard/ash、nic/nell、rin/rue、dee/dot 由 Orchestrator 按 §2.3 模拟执行，保持 ownership / review / artifact 边界。原因：同仓库内并行分支需要 worktree 隔离，sub-agent 数量受上下文与协调成本约束，其余 pod 的产出以 artifact 边界模拟。
- 事件日志写入权：**仅 Orchestrator**（避免并行 sub-agent 的 seq 冲突）。Pod 产出 worklog / 代码 / review 草稿，Orchestrator 收集后统一追加事件。
- 协议注入路径：RFC-0004（事件模型 v1.1）落地前，本 Run 的 `project.state_discrepancy` / `run.*` / `mentor.*` / `learn.*` / `ambient.*` 事件暂以 markdown 存于本目录，落地后补记（见 `discrepancies.md`）。

## P0 发现的基线事实（D1）

- 仓库已是 git 仓库并托管于 GitHub（私有）：Owner 在上一会话完成了 ISS-001 的 git init / seed commit / 远程托管（事件 seq 31–33）。
- ISS-001 剩余：工具链选型文档、CMake 骨架、docs/build.md —— 本 Run 由 ver/vic 完成（🎓 转 Learning Hook，见 discrepancies）。
- 环境：VS2022 Community @ `E:\`（含 CMake 3.31.6）· MinGW gcc 8.1.0 · Python 3.14.6 · ninja 1.10.2 · git 2.40 · GitHub 经本地代理 127.0.0.1:7897 可达（直连 000/301）。
- 无任何 C++ 代码。基线 LOC = 0。

## 疼痛点预测（继承 M0.md P1–P5，retrospective 对账）
