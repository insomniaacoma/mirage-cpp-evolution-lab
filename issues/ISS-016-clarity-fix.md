# ISS-016: 清晰度修复（字体 / DPI / 对比度）

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | feature |
| Owner | rin/rue（🎓Hook 合法形态） |
| Reviewer | **Owner**（视觉判定权，ADR-0005 惯例延续） |
| Status | open |
| Depends on | ISS-015 |

## 需求 / 验收标准

- [ ] 据 ISS-015 根因排序表逐项修复（例如：换用 SDF 字体或高分辨率字体图集 / FLAG_WINDOW_HIGHDPI + 正确的 glViewport / 配色整体提亮）。
- [ ] 对比度：墙/房间/门洞在正常截图距离可一眼区分（前后对比截图入 `docs/qa/`）。
- [ ] 文字（HUD 文案）在 100% 与 125%/150% Windows 缩放下均清晰。
- [ ] Owner 亲自验收：打开游戏 10 秒内"看不清"的主观感受消失。

## Non-goals

帧节奏问题（ISS-017）；美术风格（ISS-018）。
