# ISS-003: App shell — 窗口、主循环、退出

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | feature |
| Owner | Vera |
| Reviewer | Arden |
| Status | open |
| Depends on | ISS-001, ISS-002 |

## 背景

M0 的第一个真实代码。窗口 + 主循环是一切 gameplay 的容器。

## 需求 / 验收标准

- [ ] 打开 1280×720 窗口，标题 `MIRAGE//AFTERIMAGE — M0`。
- [ ] 主循环带 delta time（帧率无关），ESC 或关窗退出，退出路径干净（无泄漏报错）。
- [ ] 清屏颜色可配置（M0 随便一个暗色）。
- [ ] 代码只涉及 `src/main.cpp`（+ 必要的少量头文件），保持扁平。
- [ ] Vera 注册 `module:app`（`module.registered` 事件，owner = member:vera）。
- [ ] Rin 过一遍：循环里有没有每帧无谓的大分配（现在应该没有，将来有的话记账）。

## Non-goals

固定时间步、帧率限制策略、暂停、全屏切换、任何游戏逻辑。

## Notes

- Vera 的私人预感（见其档案）：main.cpp 迟早 god file。本期不做预防性拆分，但 review 时记录一笔，供 R-001 对账（疼痛点预测 P1）。
- delta time 上限 clamp（比如 0.1s），防止拖窗口回来时玩家穿墙。
