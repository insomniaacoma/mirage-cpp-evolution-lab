# REVIEW-0001 · PR: feature/m0-app-shell → main（ISS-003）

- Reviewer: Arden（主审，ISS-003 指定）· Rin（抽查 ADR-0002 调用面）
- 分支: feature/m0-app-shell · commits: d0d6b64, 95c25ec
- 本仓库无远程 PR 设施，review 依 RUN 协议 §4.6 以结构化工件承载。

## Round 1 — **Request Changes**

- [x] Correctness — 通过（窗口/循环/退出路径正确；dt clamp 有）
- [x] API — 通过（无新 API）
- [x] Ownership — 通过（无堆分配）
- [x] Lifetime — 通过
- [x] Thread safety — 通过（单线程）
- [x] Dependency direction — 通过（raylib 调用面当前只在 main.cpp，符合 ADR-0002）
- [ ] Tests — **问题**：`--smoke` 语义是"可脚本化验证"，但当前实现是**隐藏窗口的无限循环**：跑 `--smoke` 的 CI/脚本会永远挂起。守卫缺失。
- [x] Performance — 通过（Rin 抽查：循环内无分配）
- [x] Maintainability — 通过
- [x] Documentation — 通过（build.md 覆盖）

**Verdict: Request Changes** —— `--smoke` 必须要么拒绝、要么有限次帧后退出，不许静默死循环。

## 修复（commit 95c25ec + 补充）

1. 加 600 帧守卫，超限打印 `SMOKE: shell guard timeout` 并退出。
2. **修复验证时发现第二个真实问题**：`timeout 12 --smoke` 仍未退出——隐藏窗口被 DWM 节流到远低于 60fps，600 帧需要 40 秒以上。修复：smoke 模式不设 TargetFPS（无节流）；真实 smoke driver（ISS-006/007）将改用固定 dt 步进，从根上不依赖墙钟。此发现已同步 DEC-RUN0001-P1-003 的假设（"脚本化输入走 update 路径"成立，但"60fps 帧节奏"假设不成立）。
3. 编译错误教训（真实发生）：raylib 5.5 从核心 API 移除了 `Clamp()/Lerp()`（移至 raymath.h），首版 `Clamp` 直接 C3861。选择本地 3 行 helper 而不是拉 raymath.h。已记 Vera worklog。

## Round 2 — **Approve**

- [x] 全项通过。smoke 守卫实测：`timeout 5 --smoke` → 打印守卫信息，exit 0。
- 残留观察项（不阻塞）：`(void)dt` 是暂时的，ISS-004 合入时消失——Dee 会盯。

**Verdict: Approve — merge。**
