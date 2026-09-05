# ISS-008: Observatory recorder v0（obslog CLI + envelope 校验）

| 字段 | 值 |
|---|---|
| Milestone | M0 |
| Type | infra（工具） |
| Owner | Mora |
| Reviewer | Dee |
| Status | open |
| Depends on | ISS-001 |

## 背景

目前 observatory 日志靠手写 JSONL（`observatory/data/log/`）。Mora 预言了手滑（疼痛点 P5）。本 Issue 交付最小工具：**append + 校验**，不做 viewer。

## 需求 / 验收标准

- [ ] `tools/obslog.py`，**仅用 Python 标准库**（理由：开发工具不应给 C++ 学习主线添依赖；`jsonschema` 第三方库被拒，手写 envelope 检查足够）。
- [ ] `obslog.py add --type <type> --actor <actor> --ref <ref>... --payload '<json>'`：校验 envelope（必填字段、type 在注册表内、actor 格式、seq 严格递增、refs 格式）后 append 到当日日志。
- [ ] `obslog.py check`：全量校验现有日志（seq 连续、envelope 合法），损坏行报告行号。
- [ ] 容忍日志文件末尾被截断的最后一行（崩溃安全，按 data-model 的规定忽略）。
- [ ] README 用法写进 `tools/`（10 分钟能上手，Mora 的标准）。
- [ ] Dee 用一条故意写坏的事件验证校验真的会拒绝。

## Non-goals

payload 级深度校验（后续 milestone）、viewer/UI、自动从 git 采集事件。

## Notes

- envelope 规范：`observatory/schema/data-model.md` + `event.schema.json`（schema_v 1）。
- 设计倾向（已在 RFC-0001 讨论中确立）：日志是唯一事实源，工具只追加，绝不改写历史行。
