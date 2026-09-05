# REVIEW-0004 · PR: feature/ow0-obslog → main（ISS-008）

- Reviewer: Dee（ISS-008 指定）· Mora（pod senior，另见 milo-mora.md）
- 分支: feature/ow0-obslog · commits: b74c1f2, 981f505
- Orchestrator 独立复验：18 测试 OK；`obslog check` 真实日志 0 error + 1 warning（seq 54 gap）；非法 payload 拒绝且零落盘。

## Dee

- [x] Correctness — 18 用例覆盖：seq 接续、非法拒绝、未知 type（add 拒 / check 警）、截断尾行容忍、CRLF strip、seq 倒退检出
- [x] 可验证性 — 测试用 tempfile，**绝不写真实日志目录**（ Orchestrator 复验过工作树干净）
- [x] envelope 校验 — actor/refs/enum 与 event.schema.json v1.1 一致
- [x] Dee 条款 — 未知 type 在 check 是 warning 不是 error ✓（ADR-0004 落地）
- [x] gap 语义 — warning 不 error，且**如实报告**了任务书与其矛盾之处（任务书写"预期 0 warning"，但规则规定 gap=warning；milo 按规则报 1 warning，没有特判历史凑数——这正是本工具存在的意义）
- 观察（不阻塞）：截断尾行计入行数统计的口径是 milo 自主拍板（对齐 wc -l），语义合理，记入 worklog。

**Verdict: Approve**

## Mora

- 报错信息质量：`--payload is not valid JSON` 附带示例与"nothing was written"——开箱即用标准达成。
- 工具哲学符合 RFC-0001：只追加、不碰历史行；`view` 占位留给 ISS-011。
- 遗留：v0.1 给 check 的 error 路径加 prescribe 文案（见 milo-mora.md Resolution）；`add` 的 --allow-unknown 未实现（本轮不需要，记 ISS-008 备注）。

**Verdict: Approve — merge。**
