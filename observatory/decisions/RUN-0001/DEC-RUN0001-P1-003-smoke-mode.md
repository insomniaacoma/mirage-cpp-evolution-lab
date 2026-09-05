# DEC-RUN0001-P1-003 · `mirage_m0 --smoke`：无头确定性冒烟模式

- Level: Decision Capsule
- Phase: P1 · Owner: Dee 提出需求 / Nico 设计 · Reviewer: Vera

## Problem
Dee 验收 M0 需要可重复、可脚本化、无需人手的运行验证；人肉跑游戏无法写进 Gate checklist 的"可复现"标准。

## Options
A) 仅人肉验收 — 不可复现，Dee 会拒。
B) 单独 test 可执行文件绕过窗口 — 测不到"渲染循环里"的真实路径。
C) 主程序内嵌 `--smoke` 模式（选定）：隐藏窗口 + 固定 dt = 1/60 + 脚本化输入驱动 + 确定性断言 + `SMOKE:` 行输出 + exit code。

## Choice & Reason
C。证据需求：ISS-006 的确定性触发要求 + Dee 的"不可验证=不存在"。脚本化输入走与真人输入相同的 update 路径，只是输入源不同。

## Consequence
main.cpp 需要输入抽象（一个 InputFrame 结构：move/look delta），真人输入与脚本输入都填充它——这是最小、合理的可测试性接缝，不算过度设计。revisit when：出现第二个输入设备或录像回放需求。

## Revisit When
Echo/Replay（确定性模拟）需求出现时，InputFrame 正是回放单元的雏形。
