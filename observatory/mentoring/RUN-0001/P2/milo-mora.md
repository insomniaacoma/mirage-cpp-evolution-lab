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

**S (mora) — response type: socratic:**
> 先对齐一个事实：`obslog check` 的**用户**是谁？是"写坏日志的那个人"，还是"三个月后要重放历史的你（或 Owner）"？如果用户是后者，那 check 的职责就不是"帮写坏的人收拾"，而是"向未来的人证明这份历史哪里可信、哪里存疑"——这两种定位会导出完全不同的工具形态。你列的选项 b（repair 命令）在这两种定位下分别意味着什么？
>
> 第二问：你说选项 c"会不会等于鼓励手改"——那把问题反过来：**唯一合法的历史修正动作**在 RFC-0001/ADR-0004 里已经存在了，是什么？（提示：它不是 edit，它也是一个 append。那 check 在报 error 的时候，能不能把"下一个合法动作"直接印在错误信息里，而不是印"正确的行应该长什么样"？）
>
> 第三问留给你：深夜赶工的人不看 README，那这条纪律的执行点应该放在**文档**里，还是放在**报错文本**里？你的 reject 路径已经在这么做了——check 的 error 路径为什么不做同样的事？

**Resolution:**（mora 记录落地结论，milo 可反驳）
- v0 维持 report-only：check 不提供、也不暗示任何"改行"路径。你的担心成立——越清楚的 diff 越是在邀请手改。
- 官方修正路径 = **append 一个新事件**（`project.state_discrepancy`，ADR-0004）：记录"第 N 行语义错误 + 正确语义是什么"，历史行保持原样。若行本身 JSON 损坏（不可解析），discrepancy 事件引用 `file:line` 即可——损坏本身也是历史事实。
- v0.1 动作（记 ISS-008 follow-up，不阻塞本轮）：check 的 error 输出末尾固定打印 prescribe："do not edit history; append a project.state_discrepancy event (see ADR-0004)"。把纪律放进步错文本，而不是 README——这是你第三问的答案，也是 Mora 的工具观。
- 选项 b/c 正式否决，理由入档。

**Verified by:** milo 本轮对真实日志 0 error + 1 warning（seq 54 gap）的**如实报告**——没有为了"干净的输出"特判历史，这就是该工具未来被正确使用的文化证据。v0.1 的 prescribe 文案落地后，由 `tools/test_obslog.py` 增加 check error 输出断言固定之。
