# Lookup Trail 001 — milo · RUN-0001 · P2

- Member: `member:milo` · Date: 2026-09-06
- Task context: ISS-008 obslog v0（envelope 校验怎么落地）
- 总 timebox：约 6 步 / 40 分钟，全部为本会话真实执行，无凭记忆断言

## Step 1 — envelope 校验要不要用 `jsonschema` 库？

- **unknown**: 仓库里已有 `observatory/schema/event.schema.json`（draft-07），第一直觉是"有 schema 就该用 schema 校验器"，否则那份 JSON Schema 是不是白写了？
- **found**: 三条证据叠加。① ISS-008 明文记录了这个决定：「`jsonschema` 第三方库被拒，手写 envelope 检查足够」，理由是开发工具不给 C++ 主线添依赖；② 实际看 schema：1 个 object、11 个 property、必填 6 个、格式约束全是 4 个正则 + `const 1` + `type` 枚举，手写就是 `envelope_problems()` 里约 70 行直白 if；③ 手写的意外好处——schema 的 `additionalProperties: false` 我可以降级成 warning（向前兼容），JSON Schema 库没有这种"部分字段放宽"的干净写法。
- **credibility**: ①是仓库内已裁决的文档（最高）；②③是我对本仓库 schema 文件的实读。
- **adopted**: 是。stdlib 手写，代价 = 新增一条漂移风险 → 用 `test_event_types_match_schema_file` 让代码内 `EVENT_TYPES` 和 schema enum 每次跑测试对账。
- **遗留别扭**: `additionalProperties: false` 在 schema 里仍是 false，我的实现是 warning——schema 文件和实现的严格度有偏差，写进了 worklog 的 What I Still Don't Get。

## Step 2 — `datetime.fromisoformat` 能否既认 `Z` 又拒掉无时区的时间戳？

- **unknown**: envelope 要求 ts "ISO8601 with UTC offset"。手写正则太容易漏（毫秒、`T` 大小写、offset 格式）；担心 stdlib `fromisoformat` 认了 `Z` 却对无时区字符串也放行。
- **found**: 本机 Python 3.14.6 实跑：
  ```
  '2026-09-05T09:00:00+08:00' -> parsed, tzinfo = UTC+08:00
  '2026-09-05T09:00:00Z'      -> parsed, tzinfo = UTC
  '2026-09-05'                -> parsed, tzinfo = None
  '2026-09-05T09:00:00'       -> parsed, tzinfo = None
  ```
- **credibility**: 本机实跑验证（官方文档口径一致，但以实测为准）。
- **adopted**: 是。判定式 `tzinfo is not None and utcoffset() is not None`，三行解决，`Z` 和 `+08:00` 都收，裸日期/裸时间被拒。

## Step 3 — argparse 子命令缺省时的行为

- **unknown**: 裸跑 `python tools/obslog.py`（没有任何子命令）会怎样？崩溃栈？静默？退出码是多少？
- **found**: 实跑验证，`subparsers(dest=..., required=True)` 下：
  ```
  usage: demo [-h] {add,check} ...
  demo: error: the following arguments are required: {add,check}
  SystemExit code: 2
  ```
- **credibility**: 本机实跑。
- **adopted**: 是。退出码 2 与 `view` 占位的 exit 2 恰好语义一致（"不是你的错，是工具没这个功能/用法"），不额外造退出码。

## Step 4 — `Path.write_text` 能不能控制换行符（CRLF 测试夹具需要）

- **unknown**: 测试要造真正的 CRLF 文件来验证 DEC-RUN0001-P1-002 的 strip-\r。`open()` 有 `newline=""`，但测试里用的是 `write_text`，不确定它透传不透传。
- **found**: `inspect.signature(Path.write_text)` → `(self, data, encoding=None, errors=None, newline=None)`（3.10+ 增加）。
- **credibility**: 本机 introspect 实证。
- **adopted**: 是。夹具统一 `write_text(text, encoding="utf-8", newline="")`，换行完全由我拼的字符串决定，不被平台偷偷翻译。

## Step 5 — Windows 子进程管道的输出编码

- **unknown**: 工具错误信息会嵌入日志里的值，`ensure_ascii=False` 的世界迟早有非 ASCII 进 stderr；子进程/管道下 Python 默认用什么编码解码？
- **found**: 印象是 Windows 非 console 场景用 locale 编码（本机 cp936），未深挖文档；采取防御式双保险：测试子进程统一 `encoding="utf-8"`，工具侧 `sys.stdout/stderr.reconfigure(errors="replace")` 兜底防崩。
- **credibility**: 半查证半习惯（本条诚实标注：没有翻官方文档原文，属于防御性工程，不作为结论性知识）。
- **adopted**: 是（作为防御措施，不是知识断言）。

## Step 6 — 两次测试失败的实际排查（不是查外部，是查自己）

- ① 截断尾行测试期望 `1 lines`，工具报 `2 lines` —— 争议点在统计口径：坏行物理存在，统计含它才对得上 `wc -l`。判定为测试期望错，改测试。
- ② "中间坏行报 error" 测试把坏行写在了文件**末尾**——那它就是最后一行，按规则就该是 warning。修正夹具：坏行前后各放一行好行。
- 两个都是"假设走错了"，工具行为本身正确。教训：**先确认夹具真的构造出了你想测的场景，再断言**。
