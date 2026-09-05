# OW0 Wave Checklist（ISS-014 · Dot，Mora 复核）

| # | 步骤 | 预期 | 实际 | ✓ |
|---|---|---|---|---|
| 1 | `python tools/obslog.py check` | 0 error；已知 gap = warning | P2 已验 ✓（0e 1w） | ☑ |
| 2 | `python tools/obslog.py view` | 生成 data.js，事件数与 check 行数一致 | P2 已验 ✓（105） | ☑ |
| 3 | 浏览器打开 observatory/view/index.html | 七个 tab 全部渲染无 JS 错误 | P2 已验 ✓（3 bug 发现并修复） | ☑ |
| 4 | Studio：checkpoint P0/P1 切换 | avatar 位置/状态随 checkpoint 变化；kai dormant 半透明 | 待 P3 回归 | ☐ |
| 5 | Studio：点击 avatar | 侧栏显示 state/location/owned issues/recent events | 待 P3 | ☐ |
| 6 | Work：M0/OW0 双板 | 卡片状态与 INDEX.md 一致 | P2 已验 ✓（14 cards） | ☑ |
| 7 | Timeline：拖动游标 | 事件按 logical_seq 重放，issue 计数随游标变化 | 待 P3 | ☐ |
| 8 | Discussions：D-0001..0004 逐步回放 | stance chip、分歧保留、outcome+ADR | P2 已验 ✓（D-0002） | ☑ |
| 9 | Metrics：快照对比 | v0.0.0-seed 指标可见 | P2 已验 ✓ | ☑ |
| 10 | Process Lab：双板 + checkpoint 表 | gate 状态列真实 | 待 P5（gate 数据 P4 后才有） | ☐ |
| 11 | Show Fixture 开关 | 默认关；开时 fixture 内容带 FIXTURE 角标 | 待 P3 | ☐ |
| 12 | ambient 过滤 | Gate/Metrics/Lab 视图无 ambient 事件 | 待 P3 | ☐ |

## Wave Review 输入（P6 收集）

- 真实数据视图：Studio(部分)/Work/Timeline/Discussions/Metrics — source=repository
- fixture 视图：studio_live（成员实时编辑文件）、CI
- event model 压力点：P6 记录
