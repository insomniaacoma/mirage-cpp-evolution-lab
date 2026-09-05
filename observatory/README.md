# Project Observatory

> 项目的一级功能（charter P3）：可视化整个软件系统的成长过程——它什么时候开始复杂、哪个模块何时出生、错误依赖何时出现、为什么重构、重构后结构变了什么。

## 定位

Observatory = **append-only 事件日志**（事实源）+ **只读投影**（未来的六个视图）。今天只有前者；后者随真实数据与真实需求逐步出生。

## 六个视图的路线图

| 视图 | 需要的数据 | 当前状态 |
|---|---|---|
| Studio View（虚拟工作室，Avatar 位置/任务） | `member.onboarded / location_changed / task_changed` | 数据模型已定义；UI 未开始 |
| Interaction Graph（协作网络） | `interaction.logged` + `discussion.*` | 数据模型已定义 |
| Architecture Graph（模块/依赖/热点） | `module.registered / dependency_changed` | 数据模型已定义；第一个模块随 ISS-003 出生 |
| Development Graph（Issue→RFC→ADR→PR→Release 链） | `issue.* / rfc.* / adr.* / pr.* / commit / release` | **已在记录**（种子日志） |
| Time Machine（按版本回看一切） | `metrics.snapshot` + 事件重放 | 数据模型已定义；首个快照 v0.0.0-seed 已写入 |
| Discussion Replay（重放设计讨论） | `discussion.opened / turn / closed` | **已在记录**（D-0001 可回放） |

## 现在怎么记录（ISS-008 落地前的手工方式）

1. 打开 `observatory/data/log/` 下当天文件（没有就按 `YYYY-MM-DD-<topic>.jsonl` 新建）。
2. 按规范追加一行 JSON（`seq` 接着最大值递增）。
3. 格式与事件注册表：[schema/data-model.md](schema/data-model.md)；JSON Schema：[schema/event.schema.json](schema/event.schema.json)。

示例：

```json
{"seq":42,"ts":"2026-09-06T14:30:00+08:00","schema_v":1,"type":"issue.status_changed","actor":"member:vera","refs":["issue:ISS-003"],"payload":{"from":"open","to":"in-progress"}}
```

## 铁律（ADR-0001）

- 只追加，永不改写历史行。
- 视图是只读投影。
- 事件类型/schema 的演化走 ADR。
- 没有记录 = 没有发生。
