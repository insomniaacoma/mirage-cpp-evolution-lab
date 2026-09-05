# tools/

开发工具目录。约定：工具只追加、绝不改写 `observatory/data/log/` 的历史行；工具必须 10 分钟能上手（Mora 的标准）；一律只用 Python 标准库。

## obslog.py — Observatory 事件日志 recorder（ISS-008）

对 `observatory/data/log/*.jsonl`（append-only 事件日志）做追加与校验。格式规范见 `observatory/schema/data-model.md` 与 `observatory/schema/event.schema.json`（schema_v 1，含 ADR-0004 的 v1.1 可选字段）。要求 Python 3.11+（仓库标准 3.14），无第三方依赖。

### add — 追加一条事件

```bash
python tools/obslog.py add \
  --type issue.status_changed \
  --actor member:vera \
  --ref issue:ISS-003 \
  --payload '{"issue_id":"ISS-003","from":"open","to":"in-progress"}'
```

- 默认写入 `observatory/data/log/<今天YYYY-MM-DD>-tools.jsonl`；`--file PATH` 可指定其他目标文件（父目录不存在会自动创建）。
- `seq` 自动计算：扫描全部 `observatory/data/log/*.jsonl` 取最大值 +1，不用手填；`ts` 默认当前本地时间（ISO8601 带时区偏移）。
- v1.1 可选字段：`--run-id RUN-0001`、`--phase-id P2`、`--logical-seq 1`、`--process-mode pair`。
- `--ref` 可重复：`--ref issue:ISS-008 --ref member:mora`。
- 校验失败（未知 type、actor/ref 格式错、payload 不是 JSON 对象等）会**一次性列出所有问题**且不写任何东西，exit 1。

cmd.exe 下 payload 的引号转义与 bash 不同：`--payload "{\"key\":\"value\"}"`。

### check — 全量校验现有日志

```bash
python tools/obslog.py check
```

- 按文件名顺序扫描 `observatory/data/log/*.jsonl`，逐行做 envelope 校验；同时检查全局 `seq` 单调（跳号 = warning，重复/倒退 = error）与按 `run_id` 分组的 `logical_seq` 单调（倒退 = error）。
- 未知 `type` 只报 warning 不报 error（向前兼容，Dee 的条件）；`schema_v != 1` 是 error。
- 文件最后一行 JSON 解析失败视为截断尾行：warning 并跳过（崩溃安全，DEC-RUN0001-P1-002）；中间行解析失败是 error。
- 行尾：读取方按 DEC-RUN0001-P1-002 容忍 `\r`（Windows CRLF 工作区）；写入方固定写 LF。
- 结尾打印每文件统计；有 error 时 exit 1，只有 warning 时 exit 0。

### view — 只读投影（占位）

```bash
python tools/obslog.py view
# not implemented yet (ISS-011)，exit 2
```

### 退出码

`0` 成功（check 允许带 warning）· `1` 校验失败或运行错误 · `2` 用法错误 / view 未实现。

### 测试

```bash
python tools/test_obslog.py -v
```

测试全部在临时目录里通过 `--root` 隔离运行，**绝不读写真实的 `observatory/data/log/`**。其中一条测试会只读校验 `obslog.py` 内置的事件类型注册表与 `event.schema.json` 是否一致，防止两处漂移。

### 其他

- `--root PATH`：指定仓库根目录，默认从脚本自身位置推断（所以在任何工作目录下跑都指向本仓库）。测试用它隔离。
- v0 已知限制：并发的两次 `add` 可能算出相同的 `seq`（单人使用场景，暂不处理）；坏行只被 check 报告，工具不做任何修复——日志是唯一事实源，改历史必须走公开流程。
