# ISS-015: 帧时间与模糊归因（测量，不改行为）

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | investigation |
| Owner | rin/rue（🎓Hook 合法形态） |
| Reviewer | Dee |
| Status | open |
| Depends on | — |

## 背景

Owner 实测："画面模糊对比度不高，帧率低延迟高……卡顿严重"。charter P1 纪律的反面同样成立：**修复也要证据先行**。本 Issue 只测量、只归因，不改行为。

## 需求 / 验收标准

- [ ] `--smoke` 与正常模式输出帧时间分布（p50/p95/max，≥1000 帧样本），写入 `docs/qa/M1-perf-baseline.md`。
- [ ] "模糊"归因三选一（或组合）：① raylib 默认字体纹理过滤（DrawText 小字号发糊）；② Windows DPI 缩放位图拉伸；③ 渲染分辨率 vs 窗口尺寸不匹配。各给证据（截图放大对比 / SetConfigFlags 实验记录）。
- [ ] "卡顿"归因：逐帧 DrawCube（36 盒，理论上不该卡）vs DWM 合成节奏 vs dt 节奏。用排除法：每项一次最小实验（如 SetTargetFPS(0) / SwapBuffers 间隔打点）。
- [ ] 结论写成"根因排序表"（最可能 → 最不可能，各附证据），供 ISS-016/017 精确打击。

## Non-goals

任何修复性改动；优化代码。

## Notes

- Rin 的 watchpoint：先量 frame time，再说话。
- 疼痛点预警（M1.md）：DrawText 发糊是头号嫌疑（默认字体是 8px 位图字体，纹理过滤放大发糊——查证 raylib Github issues 可作为 Lookup Trail 素材）。
