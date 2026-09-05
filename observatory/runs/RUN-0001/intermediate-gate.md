# RUN-0001 · Intermediate Gate（P3 首次汇合 · Dee 主持）

- Date: 2026-09-06（logical: P3）· Chair: Dee · Attendees: 全体活跃 pod
- 输入：main @ a2e23b1（gameplay merge + QA 探针）

## 检查项与结论

| 项 | 方法 | 结果 |
|---|---|---|
| build | 全量重新配置+构建 | **PASS**（0 error；一次 LNK1104 为残留游戏进程占用 exe，环境问题非代码问题） |
| runtime | `mirage_m0.exe --smoke` ×3 | **PASS**（输出逐字节一致：mutation@498 v9→11，exit@809，PASS，exit 0） |
| world graph | `test_worldgraph.exe` | **PASS**（6/6） |
| movement | smoke 物理走位（脚本输入走真人路径） | **PASS**（A→B 触发→D→E 全程无穿透；squeeze 探针 16 checks） |
| observatory | obslog check + view + 七视图浏览器实测 | **PASS with fixes**（本轮发现 4 个真实 UI bug，全部已修复：checkpoint 字段名、location 默认回填、role 颜色、tab 高亮） |
| event consistency | obslog check：105 事件 | **PASS**（0 error, 1 warning=已知 seq 54 gap） |
| merge conflicts | worldgraph/play 两次合入 | **resolved**（CMakeLists 两处手工合并，已记录） |
| architecture pressure | 人工检查 | **记录**：main.cpp 仍吸收输入采集+接线（P1 预测 god file 记账 +1）；render 调用面守 ADR-0002 |

## Gate 中发现并已修复的问题（真实缺陷，非表演）

1. **smoke 截图全黑**：`TakeScreenshot` 读窗口 framebuffer，`FLAG_WINDOW_HIDDEN` 下永远黑 → 改 `RenderTexture` 离屏渲染 + `ExportImage`（commit a2e23b1）。
2. **玩家相机贴墙无目检价值**：终点状态贴墙 → QA 固定俯视相机。
3. **UI 三处投影 bug + tab 高亮**（见 OW0-checklist #3，commit 0cd38c8 / bbbfa27）。
4. **LNK1104**：环境卫生问题，记入 build.md 已知坑（待写）。

## 未决（转 P4/P5）

- 人肉手感类 checklist（B1-B4、C3 察觉率、D3）：**无法自动化，Owner 未在场** → Gate 判定为 *PASS with deferrals*：自动化路径全绿，手感项列为 Owner 交付时验收项。Dee 声明：这不阻塞 v0.1 tag（M0 DoD 的"可玩"由确定性走位 + 几何证据支撑），但"手感调优"列入 R-001 交付建议。
- C3 的"察觉率"问题：Kai 预留的 retro 议题。

## 结论

**Intermediate Gate: PASSED**（带 2 项 deferral 记录）→ 进入 P4（无阻塞性 rework，收尾事项）→ P5 Final Gate。
