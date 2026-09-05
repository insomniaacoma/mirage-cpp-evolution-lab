# Vera — P2 Worklog（ISS-003 App Shell）

## Current Task
ISS-003：窗口 + 主循环 + 干净退出。分支 feature/m0-app-shell。

## Current Judgment
M0 的 app shell 就该是 60 行：窗口、clamp 过的 dt、ESC/关窗退出。任何更多（时间步固定、渲染抽象、状态机）都是替还不存在的需求写代码。

## Options Considered
- 固定时间步累加器（accumulator）现在就上：**拒绝**——M0 没有需要确定性的物理，dt clamp 到 0.1s 已覆盖拖拽窗口的最坏情况。固定步长等 smoke/物理真需要时再加（Dee 的 smoke 需求由 DEC-RUN0001-P1-003 承载）。
- 把 InputFrame 结构现在定义：**拒绝**——这是 Nell 的控制器接口，我提前定了就是替别人设计 API。

## Implementation
- `--smoke` 解析 + 隐藏窗口 + 600 帧守卫 + 非 smoke 才 SetTargetFPS(60)。
- clampf 本地 helper（raylib 5.5 删了核心 API 的 Clamp）。

## Debug / Investigation
- **C3861 'Clamp'**：raylib 5.5 把 Clamp/Lerp 移出核心 API（查证：头文件 grep 无 RLAPI Clamp，5.5 changelog 确认移入 raymath.h）。本地 helper 解决。Lookup Trail 已记。
- **隐藏窗口 DWM 节流**：600 帧守卫在 `timeout 12` 内不退出。假设"60fps 帧节奏"被证据推翻（rejected by evidence）——实测隐藏窗口帧率被压到远低于 60。修复：smoke 不节流。**这个发现会传给 Nell：smoke driver 必须固定步长，不依赖 GetFrameTime 的节奏。**

## Verification
- `timeout 2 mirage_m0.exe` → 124（持续运行到 kill，正常）。
- `timeout 5 mirage_m0.exe --smoke` → 打印守卫信息，exit 0。

## Review Response
REVIEW-0001 Round 1 Request Changes（smoke 死循环）→ 加守卫 + 节流修复 → Round 2 Approve。接受，Arden 抓得对：脚本化的东西死循环是我该想到的。

## Remaining Concerns
- main.cpp 开始吸收游戏逻辑（draw 里已有文案）——P1 预测的 god file（疼痛点 P1）记账 +1，暂不动作。
