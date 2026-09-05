# M0 Smoke Checklist（ISS-009 · Dee）

> 使用方式：每条按「预期 / 实际 / 通过」记录。自动化部分以 `--smoke` + `test_worldgraph` 为准；
> 手感与视觉部分 P3/P5 由 Dee 亲手跑（不许只信 Nell 的演示）。
> 确定性要求（M0.md）：mutation 触发无随机，任何失败必须可复现。

## A. 构建与启动

| # | 步骤 | 预期 | 实际 | ✓ |
|---|---|---|---|---|
| A1 | 按 docs/build.md 全新 clone 路径配置+构建 | 0 error | 待 P3 | ☐ |
| A2 | `test_worldgraph.exe` | all checks passed, exit 0 | 待 P3 | ☐ |
| A3 | `mirage_m0.exe`（正常模式） | 1280×720 窗口，标题正确，60fps | 待 P3 | ☐ |
| A4 | `mirage_m0.exe --smoke` | 打印 SMOKE 序列，exit 0 | 待 P3 | ☐ |

## B. 移动与碰撞（ISS-004）

| # | 步骤 | 预期 | 实际 | ✓ |
|---|---|---|---|---|
| B1 | WASD 四方向 + 对角 | 相对朝向移动，速度帧率无关 | 待 P3 | ☐ |
| B2 | 鼠标转向 | yaw 无限制，pitch ±89° clamp | 待 P3 | ☐ |
| B3 | 撞墙走墙 | 不穿墙，沿墙滑动不卡角 | 待 P3 | ☐ |
| B4 | 拖拽窗口再回来 | 不瞬移（dt clamp 生效） | 待 P3 | ☐ |

## C. 世界与突变（ISS-005/006）

| # | 步骤 | 预期 | 实际 | ✓ |
|---|---|---|---|---|
| C1 | 巡视 5 个房间 | 全部可达（初始拓扑），E 不可达 | 待 P3 | ☐ |
| C2 | 进入 B 累计 5 秒 | 突变恰好一次；C-D 门封死；D-E 门出现 | 待 P3 | ☐ |
| C3 | 突变瞬间 cue | 全屏闪 + 文案，可察觉（Kai 的察觉率问题记入 retro） | 待 P3 | ☐ |
| C4 | 突变时站在 C-D 门洞 | 被推出，不穿墙不卡死 | 待 P3 | ☐ |
| C5 | 重复进出 B 计时 | 触发语义符合 Nell 的设计说明（累计 or 连续，与文档一致） | 待 P3 | ☐ |

## D. 出口与闭环（ISS-007）

| # | 步骤 | 预期 | 实际 | ✓ |
|---|---|---|---|---|
| D1 | 突变前走到 E 门口 | 被墙挡住（拓扑封死，非视觉隐藏） | 待 P3 | ☐ |
| D2 | 突变后 B→D→E | 路径通畅 | 待 P3 | ☐ |
| D3 | 踏入 E | "LAYER 01 - CLEARED" 反馈清晰 | 待 P3 | ☐ |
| D4 | ESC 退出 | 进程干净退出 | 待 P3 | ☐ |

## E. 冒烟脚本断言（自动化）

`--smoke` 必须：脚本输入走真人 update 路径；断言 mutation 一次、exit 可达；`SMOKE: PASS` exit 0。
（P3 起附加：smoke 末帧 TakeScreenshot 落盘，供几何目检。）

## Gate 规则

- 任何一条 ✗ → 建真实 bug Issue，进 P4 Rework，修复后重跑该节。
- Dee 有权整体宣布 Gate Failed（尤其"可察觉"类主观项）。

---

## 实测记录（RUN-0001 · P3 Intermediate Gate + P5 Final Gate · Dot 执行、Dee 复核）

- A1 构建实测：clean build+configure **9 秒**，0 error 0 warning（VS2022 / CMake 3.31.6）。
- A2 实测：`test_worldgraph` 6/6 all checks passed，exit 0。
- A3 实测：正常模式窗口运行正常（早期 orb cam 阶段与现玩家相机阶段均验证；人肉手感项 B1-B4 **deferred to Owner**）。
- A4 / E 实测：`--smoke` 连跑 5 次输出**逐字节一致**：`mutation applied at frame 498 (graph version 9 -> 11)`、`exit reached at frame 809`、`SMOKE: PASS`，exit 0。
- C1/C2/C5/D1/D2 自动化覆盖：smoke 脚本输入物理走位（与真人共用 update 路径）——B 房累计触发、C-D 封死、D-E 开启、E 抵达全部由断言保证；拓扑由行为证明。
- C4 实测：squeeze 探针 16 checks（fire 帧门洞挤出的确定性、双向 120 帧不可穿越、单帧位移 ≤0.5u）。
- 几何目检：`observatory/snapshots/RUN-0001/smoke-final-state.png`（离屏渲染俯视图，5 房间/门洞/封死的 C-D 与新开的 D-E 可辨）。
- B2/B3/B4、C3（察觉率）、D3 主观项：**deferred**（Owner 交付验收 + Kai retro 议题）。
