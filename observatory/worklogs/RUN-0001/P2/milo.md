# Milo — Worklog · RUN-0001 · P2

- Member: `member:milo` (junior, Tools/Editor pod, senior: Mora)
- Date: 2026-09-06 · Scope: ISS-008 observatory recorder v0
- Branch: `feature/ow0-obslog` · Commit: code `b74c1f2`, obs artifacts follow-up commit

## Current Task

ISS-008：`tools/obslog.py` v0 —— 三个子命令 `add` / `check` / `view`，只用 Python 标准库，配 stdlib unittest 测试与 `tools/README.md`。验收里 Dee 那条（"用一条故意写坏的事件验证校验真的会拒绝"）我在测试里覆盖了四类坏事件（坏 payload、未知 type、schema_v 错、seq 倒退），都真的被拒了。

## Current Judgment

- **校验宁可严在 add、宽在 check。** add 拒绝一切不在注册表里的 type；check 对未知 type 只给 warning（ADR-0004 / Dee 的条件）。这样老日志永不过期，新事件进门时就被拦住。
- **报错要一次说完。** add 把所有问题收集齐一起报（`--actor` 和 `--ref` 都错就报两条），附一句 "nothing was written"。Mora 说过"第一次用就报错 = 工具不存在"，那报错至少要让人一次改对。
- **工具默认值锚定仓库根**（从脚本自身位置推断），在任何工作目录跑都指向本仓库的 log；测试用 `--root` 指到 temp，实现物理隔离。
- **check 的统计行数 = 数据行数**（含被容忍的截断尾行）。截断行物理上存在，统计里藏掉它会让人对不上 `wc -l`。这是我做的语义选择，没跟 Mora 对过。

## Options Considered

1. **argparse vs 手写解析** → argparse。手写省不了多少代码，却要自己处理 `--ref` 重复、类型转换、usage 输出；argparse 的 usage error exit 2 正好和 `view` 占位的 exit 2 语义一致。查证过 3.14 下 `subparsers(required=True)` 裸跑会给出清晰的 `the following arguments are required: {add,check,view}`。
2. **校验读 schema 文件 vs 硬编码注册表** → 硬编码 `EVENT_TYPES` frozenset + **一条测试去读真实的 `event.schema.json` 对账**。理由：check/add 不该依赖 schema 文件在 `--root` 下的存在（测试的 temp root 里没有 schema 文件），但完全硬编码又会和 schema 漂移——用 `test_event_types_match_schema_file` 把漂移变成测试失败。两个世界都要。
3. **check 发现坏行时的容错边界**：末行坏 = warning（DEC-RUN0001-P1-002 / data-model §1 崩溃安全），非末行坏 = error。中间行坏不能当截断忍，那是真损坏。
4. **写入行尾**：写死 LF（`open(..., newline="\n")`），读取侧按 DEC 容 `\r`。DEC 说"写如所写"，但既然工具是唯一写入方，固定 LF 让库内文件和工作区文件一致，少一层心智负担。

## Implementation

- `tools/obslog.py`（~450 行）：`envelope_problems()` 是 add 和 check 共用的校验核心，返回 `(severity, message)` 列表——unknown type / unknown envelope field 给 warning 级，其余 error 级。add 在写盘前对组装好的 envelope 再跑一遍它，保证两个命令对"合法"的定义永远一致。
- seq 分配：`scan_max_seq(log_dir) + 1`，扫描容错（坏行跳过，坏文件的报告留给 check）。
- check：全局 seq 用 max 追踪（5→4→5 会正确报 backwards + duplicate，而不是被 5 抹平）；logical_seq 按 run_id 分组。
- `tools/test_obslog.py`：18 条测试，全部经子进程 + `--root` 指向 temp，真实 log 目录零接触（有 git status 断言验证过）。

## Verification（真实输出）

```
$ python tools/test_obslog.py -v        （尾部）
test_check_warns_on_seq_gap ... ok
test_event_types_match_schema_file ... ok
test_view_is_a_stub_with_exit_2 ... ok
----------------------------------------------------------------------
Ran 18 tests in 1.600s
OK

$ python tools/obslog.py check
WARN  observatory\data\log\2026-09-06-run0001.jsonl:21  seq gap: expected 54, got 55

observatory\data\log\2026-09-05-seed.jsonl: 30 lines, 0 error(s), 0 warning(s)
observatory\data\log\2026-09-06-git-hosting.jsonl: 3 lines, 0 error(s), 0 warning(s)
observatory\data\log\2026-09-06-run0001.jsonl: 36 lines, 0 error(s), 1 warning(s)
total: 3 file(s), 69 lines, 0 error(s), 1 warning(s)
result: OK
```

真实日志 0 error；唯一 warning 是已知的 seq 54 gap，按规则报 warning 不报 error，exit 0。smoke：对 temp 目录 add 合法事件（seq 正确接续为 2，紧凑 JSON、LF、带 v1.1 字段）、`--payload '[1,2]'` 与未知 type 均被拒（exit 1，提示 "nothing was written"）、`view` exit 2。事后 `git status observatory/data/log/` 干净——本会话没碰真实日志。

## Remaining Concerns

- **AGENTS.md 的记录义务和本次任务禁令打架**：守则说会话造成的每个状态变化都要追加 observatory 事件，但任务明确禁止我写 `observatory/data/log/`（连测试 add 都必须 `--file` 到 temp）。我遵守了禁令，于是本会话的两次 commit 目前**没有**对应的 `commit.recorded` 事件——需要 Owner 或下个有权限的会话补记。
- 并发 add 会算出同一个 seq（v0 已知限制，README 已注明）；单人工作流下风险低。
- add 的 seq 扫描会漏掉截断尾行里的 seq——理论上可能分配出与截断行重复的号。概率低，先记着。
- 任务书里"预期 0 error、0 warning"与"gap 是 warning 不是 error"自相矛盾；我按后者实现（gap 报 warning），真实日志因此是 0 error + 1 warning。已如实报告，没有为了凑"0 warning"去特判历史 gap。
- `"1 lines"` 这种单复数没处理，输出是工程风格的，不是文案级别的。不想为这个加代码。

## Stuck

（无遗留卡点。中途两次测试失败均为测试自身构造错误，见 What I Looked Up / 下面两条：截断行统计口径、坏行误写在文件末尾。各 10 分钟内解决。）

## What I Looked Up

1. 截断尾行该不该计入行数统计——自己拍板"计入"，理由见 Current Judgment；没查到外部先例，属于工具语义自定。
2. `subprocess` text 模式在 Windows 用 locale 编码（cp936），工具输出将来可能有非 ASCII → 子进程统一 `encoding="utf-8"`，工具侧 `reconfigure(errors="replace")` 兜底。
3. 其余查证见 `observatory/learning/RUN-0001/milo/lookup-001.md`。

## What I Still Don't Get

- **seq 到底该不该在 add 时防并发？** v0 假设单人。但如果未来 run protocol 让多个 subagent pod 同时记日志（run0001 已有 4 个 pair），max+1 就会撞号。加锁？加文件锁就是新复杂度；不加，撞了之后 check 报 duplicate，谁来修？
- **payload 级校验（ISS-008 non-goal 之外的部分）推进的时机**：现在 payload 只查"是 object"。`issue.status_changed` 缺 `from/to` 也能过。data-model §5 说"上表 payload 约束由 ISS-008 的 validator 落实"，但 ISS-008 的验收标准里没有 payload 深度校验（non-goal）。这两份文档我读不出一致结论，记下问题等裁决。
- **`schema_v=1` 下的 additionalProperties**：schema 写了 `additionalProperties: false`，但我对未知 envelope 字段给了 warning 而不是 error——v1.1 自己就是"不升 schema_v 加字段"的先例，error 会把未来的合法日志拒之门外。这个选择对不对，想让 Mora 或 Dee 确认。
