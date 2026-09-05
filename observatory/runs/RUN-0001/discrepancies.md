# RUN-0001 · project.state_discrepancy 记录

> 事件类型 `project.state_discrepancy` 由 RFC-0004 引入；以下记录在其落地后补记为正式事件（见 event log）。

## SD-1 · ISS-001 的 🎓 状态与 Run 协议冲突

- **Owner expectation**（上一轮）：ISS-001（git init、工具链、CMake 骨架）整体留给 Owner 亲手完成。
- **Repository evidence**（本 Run P0）：git init / seed commit / GitHub 托管 **已由 Owner 完成**（event seq 31–33，2026-09-06）；ISS-001 处于 in-progress，剩余工具链文档 + CMake 骨架 + build.md。
- **Adopted baseline**：采用仓库状态。剩余部分由 ver/vic 在本 Run 完成——依据：Owner 在 RUN-0001 协议 §5 DoD 要求"可构建可运行"，且 §4.8 规定此类任务由 Agent 实现并产出 Learning Hook（替代"留给 Owner 亲手做"的原约定）。ISS-005（WorldGraph）同样适用：Agent 实现 + `docs/learning/` Hook + Guess 模式回放素材。
- **不改写历史**：ISS-001 文件不删除 🎓 标记历史，只追加状态注释。

## SD-2 · 网络拓扑

- Owner expectation：可直接访问 GitHub。
- Repository evidence：event seq 33 记录"间歇性直连故障，repo 配置代理 127.0.0.1:7897"；P0 实测直连 curl 000，经代理 206。
- Adopted baseline：所有外网获取（raylib 等）走 `curl -x http://127.0.0.1:7897`。已写入 Run Card。
