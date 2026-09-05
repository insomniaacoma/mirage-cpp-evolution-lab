# ISS-002: RFC-0002 — M0 的窗口/渲染方案

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | decision（T2：新第三方依赖） |
| Owner | Arden 起草 RFC → **owner 裁决** 🎓 |
| Reviewer | Rin、Vera 出意见 |
| Status | done（RUN-0001 · P1，Autonomous Project Decision） |
| Depends on | ISS-001 |

## 背景

M0 需要开窗口、画简单几何、读键鼠。这是项目的第一个第三方依赖（T2），必须走完整流程。

## 需求 / 验收标准

- [ ] Arden 创建 `docs/rfcs/RFC-0002-window-render-choice.md`，按模板填全 12 节。
- [ ] 候选至少覆盖：A) raylib；B) GLFW + glad + OpenGL 3.3；C) SDL2 + GL；D) Win32 + GL 裸写（预期被拒，写明理由）。
- [ ] Rin、Vera 的意见以讨论记录形式留在 RFC 内（允许分歧）。
- [ ] **Owner 做最终选择** → 产出 ADR-0002；RFC 状态改为 accepted。
- [ ] observatory 事件：`rfc.opened`、讨论 `discussion.turn` 若干、`adr.accepted`。

## 团队当前倾向（供裁决参考，非结论）

- **推荐 raylib**：M0 期"最短路径到可玩"，窗口/输入/几何绘制一体，CMake 友好；代价是将来换自研渲染层时这层代码是抛弃式的——**这个债要写进 ADR 明示接受**。
- B 方案学习价值更高（Owner 想深入 GL/图形管线），但 M0 时间成本约 3–5 倍，且与 M0 验收标准无关。
- 折中考虑：M0 用 raylib 快速验证玩法；若 Owner 想学图形底层，在后续 milestone 用独立学习分支/沙盒项目进行，不阻塞主线。

## Non-goals

不做渲染架构设计；不讨论 Vulkan（charter §4 禁令）。

## 裁决注记（RUN-0001 · P1）

- 结论：**raylib 5.5**（ADR-0002，Autonomous Project Decision——Owner 未裁决，依 RUN 协议 §3 由团队决策，可被 Owner 未来 ADR supersede）。
- Owner 曾问的问题已有答案：M0 只承担"验证玩法"；图形管线学习保留为后续独立路径（RFC-0002 Alternative A 未被拒绝）。
- Rin 的 review 条件已写入 ADR：raylib 调用面收敛在 `src/main.cpp` + `src/scene.cpp`。
- 证据：`.spike/` MSVC + raylib 静态链接运行 exit 0（链接组合已记录进 `docs/build.md`）。

## Notes

裁决时建议 Owner 回答一个问题：**M0 的目的是验证玩法，还是学习图形管线？** 两者都对，但不该由同一个 milestone 承担。
