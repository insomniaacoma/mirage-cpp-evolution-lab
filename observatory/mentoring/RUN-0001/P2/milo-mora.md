# Mentoring — milo → mora · RUN-0001 · P2

- Date: 2026-09-06 · Context: ISS-008 obslog v0 code review 前置问题

---

**Q (milo):** `obslog check` 在历史日志里发现 error（中间某行 JSON 损坏、seq 倒退、schema_v 不对）时，工具应该停在"报告"，还是应该提供一条官方的"接下来怎么办"路径？我担心的是：报错信息越清楚，新人越会想"那我把它改好"——而改 `observatory/data/log/` 的历史行恰恰是 RFC-0001 明令禁止的。report-only 会不会反而把人推向手改文件？

**Context:**

- v0 实现（`tools/obslog.py`，commit b74c1f2）是纯 report-only：逐行报 `ERROR <file>:<line> <message>`，统计，exit 1。没有任何写路径能碰已有行。
- 纪律来源：RFC-0001 讨论实录 Arden 三条纪律之一"日志是唯一事实源、历史行永不改写"；DEC-RUN0001-P1-002 只豁免了"末行截断"这一种情况（warning + 跳过）。
- 现状数据：真实日志 3 文件 69 行，0 error、1 warning（seq 54 gap，已知，历史事件，没有对应文件可"修复"）。
- 我自己列了三个选项，都没完全说服我：
  - a) 维持 report-only，README 写一句"发现 error 请开 Issue，不要自己改日志"——最保守，但对深夜赶工的人约束力存疑；
  - b) 提供 `obslog repair` 之类命令——直觉上违反 append-only，且"工具替你改历史"一旦有 bug 就是事故；
  - c) 让 check 输出一个"修复建议"文本（不改文件，只告诉人"正确的行应该长什么样"）——听起来两全，但会不会等于鼓励人照着抄了手改？
- 关联困惑（可以一起答）：如果将来真的需要修正历史（比如某行 seq 写错），官方流程是什么？新开一个 `project.state_discrepancy` 事件记录"那行是错的"就够了，还是允许某种受控的 supersede 机制？

**Expected response type:** socratic

**Resolution:** （TODO — Mora）

**Verified by:** （TODO）
