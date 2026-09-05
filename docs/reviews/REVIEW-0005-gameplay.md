# REVIEW-0005 · PR: feature/m0-play → main（ISS-004 / ISS-006 / ISS-007 + smoke driver）

- Reviewer: Nico（玩法主审）· Vera（跨 pod：lifetime/碰撞）· Dee（可验证性，gate 前置意见）
- 分支: feature/m0-play · commits: 1c1f04e, 2118f2e, 3a9bd61
- 证据：`--smoke` 连跑 5 次逐字节一致（mutation @frame 498, version 9→11, exit @809, PASS）；test_worldgraph 6/6；squeeze 探针 16 checks PASS；构建 0 error 0 warning。

## Nico

- [x] Correctness — 突变恰好一次（version 9→11 = disconnect+connect 各一次）；exit 门控在拓扑层非视觉层
- [x] API — InputFrame 即 DEC-003 的最小契约；输入采集留 main、解算在 player，烟测与真人共用 update 路径
- [x] 确定性 — 固定 dt、语义断言（fire 时连续停留 <4s）而非帧号断言
- [x] 手感 — 常量全部具名（4.2 u/s / 0.0025 rad/px / r=0.35 / 眼高 1.7），真人微调留给 Dee checklist
- [x] 设计说明 — dwell=累计的语义与理由在 mutation.cpp 注释 + 设计说明，M0.md 疼痛点 P3 记账
- [x] 边界情况 — 突变时玩家在门洞：探针验证确定性不穿墙；已知行为入档（见 nell-nico.md Resolution）
- **Verdict: Approve**

## Vera（跨 pod）

- [x] **失效矩阵纪律**：`player.update(in, dt, scene.solids)` 每帧从当前 scene 取参，mutation 重建后无跨帧悬垂引用（vic-vera mentoring 预言的场景，实际接线正确）
- [x] Ownership — solids 是 const 借出；player 无状态残留指向 graph/scene 内部
- [x] ADR-0002 — player/mutation/exit 零 raylib 调用；Camera3D 派生留 main；kEyeHeight 作为身体数据从 player.h 导出，边界干净
- [x] kCollisionSkin 修复 — 8.0f-0.35f 的 ~9e-8 浮点尘埃是探针实测抓到的真 bug，修复方式（skin 容差）不掩盖逻辑
- **Verdict: Approve**

## Dee（gate 前置）

- [x] 可复现 — smoke 5 连跑一致；人肉 checklist 已备（docs/qa/M0-smoke.md），手感类项目 P3 亲手跑
- [x] smoke 不画帧（PollInputEvents 泵消息避 DWM 节流）的取舍已声明——仿真路径与真人共用，接受
- **Verdict: Approve，进入 P3 中间 Gate**

**合并结论：Approve ×3 — merge。**
