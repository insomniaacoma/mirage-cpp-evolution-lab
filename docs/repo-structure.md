# Repository 初始结构建议

- Version: 1 · Date: 2026-09-05
- 原则（charter P0/P1）：结构从最小开始；**目录只在有真实内容时创建**；新增顶层目录视为 T2 决策。

## 当前结构

```
MIRAGE Cpp Evolution Lab/
├── README.md                 # 入口 + 现状快照
├── AGENTS.md                 # Agent 会话守则
├── .gitignore                # 已就绪（git init 由 ISS-001 完成）
├── docs/
│   ├── charter.md            # 宪法
│   ├── team/                 # ownership.md + members/*.md（7 人档案 + 私人笔记）
│   ├── process/              # process.md + templates.md
│   ├── milestones/M0.md      # 里程碑定义
│   ├── rfcs/                 # RFC-0001 已在
│   ├── decisions/            # ADR-0001 已在
│   └── retrospectives/       # 空；R-001 由 ISS-010 产生
├── issues/                   # INDEX.md + ISS-001..010
├── src/                      # 空。ISS-001/003 放入第一批代码；保持扁平
├── assets/                   # 空。M0 不需要资产
├── tools/                    # 空。ISS-008 放入 obslog recorder
└── observatory/
    ├── README.md             # Observatory 章程
    ├── schema/               # data-model.md + event.schema.json
    └── data/log/             # append-only 事件日志（唯一事实源）
```

## 演化政策

| 事件 | 允许的动作 |
|---|---|
| 第一个代码文件 | 直接放 `src/`（ISS-003） |
| `src/` 内出现真实的模块边界疼痛 | RFC → ADR → 拆目录/拆模块 |
| 需要资产 | 创建 `assets/<类型>/`，在 Issue 里说明 |
| 新第三方依赖 | 必须 RFC + ADR（第一个将是 RFC-0002 的结论） |
| 新顶层目录 | 必须 RFC + ADR |

## 命名约定

- C++ 文件与符号：`snake_case`（`world_graph.cpp`、`WorldGraph`）。
- 文档文件：`kebab-case`；Issue/RFC/ADR ID 大写带前缀零填充（`ISS-005`、`RFC-0002`、`ADR-0001`）。
- 分支：`iss-<id>-<slug>`。
- observatory 日志：`YYYY-MM-DD-<主题>.jsonl`，一行一事件。
