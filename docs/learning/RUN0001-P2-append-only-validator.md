# Learning Hook — append-only 日志的校验器设计（RUN-0001 · P2 · guided）

> 来源：milo 的 obslog 实现 + mora 的 socratic mentoring（全文见
> `observatory/mentoring/RUN-0001/P2/milo-mora.md`）。OW0 工具线的教学切片。

## 当时面临的问题

日志是唯一事实源（ADR-0001），但校验器在历史里发现错误时，工具的每一条输出都在教育用户"接下来该干什么"——包括教坏人。

## 关键决策链

1. **该不该有 `obslog repair`？**
   - milo 列了三个选项（report-only / repair 命令 / 打印"正确的行"）。
   - mora 的反问：check 的用户是"写坏日志的人"还是"三个月后重放历史的你"？
   - 落点：**report-only + prescribe**。错误输出末尾固定印出唯一合法动作：`append 一个 project.state_discrepancy 事件`（ADR-0004），绝不打印"正确的行长什么样"——越清楚的 diff 越是在邀请手改。
2. **校验该用 jsonschema 库吗？**
   - 约束：工具链标准库优先（ISS-008），且 envelope 校验只是"必填字段 + 正则 + enum"的子集。
   - 落点：stdlib 手写 ~60 行校验，schema 文件仍是人类可读的规范源。
3. **容忍边界**（Windows 现实）
   - `rstrip("\r\n")` 处理 CRLF（DEC-RUN0001-P1-002）。
   - 文件**末行**截断 → warning 跳过（崩溃安全）；中间行坏 → error。
   - seq gap → warning；重复/倒退 → error。已知 gap：seq 54（RUN-0001 早期写入事故，如实保留）。

## 关键代码（tools/obslog.py 的行为契约）

```
check 的输出分级：
  ERROR  → 历史不可信的部分，exit 1
  WARNING→ 已知/可容忍的偏差（gap、截断尾行、未知 type），如实计数
  prescribe 尾注 → "do not edit history; append a project.state_discrepancy event"
```

## milo 的真实错误（ Guess 模式素材）

- 测"中间坏行报 error"时，把坏行写在了**文件末尾**——按规则它合法地变成了 warning。
  教训：**先确认夹具真的构造出目标场景，再断言**。Lookup Trail 已记录。
- 统计口径：截断尾行计不计入行数？milo 拍板计入（对齐 `wc -l`）并在 worklog 说明——
  工具语义没有规定时的自主决策范例。

## 一个替代方案（被否决）

`jsonschema` 库全量校验：更"标准"，但引入第三方依赖违反工具链政策，且 payload 级校验本就推迟到后续 milestone（ISS-008 明确 non-goal）。

## Review 问题（自测）

1. 为什么"未知 type"在 `add` 是拒绝、在 `check` 只是 warning？（提示：两个命令的用户和时机不同——向前兼容）
2. 如果 seq 54 的 gap 当年被"修复"（改行补号），哪些下游投影会 silently 说谎？
3. prescribe 文案为什么放在报错文本而不是 README？（milo 的第三问）

## 建议 Owner 亲手做的小练习

给 `obslog check` 人为制造一个中间坏行（复制某行删一个字符），观察输出分级；
然后**只允许用 append** 的方式"修正"它——写一条 `project.state_discrepancy` 事件引用 `file:line`。
体会"修正历史"和"记录修正"的区别。

## 术语卡（guided）

- **append-only**：只追加不修改；审计与重放可信度的根基。
- **envelope / payload**：事件的信封字段（seq/ts/type/actor/refs）与内容体；v1 只硬校验前者。
- **prescribe ≠ suggest**：错误信息给出唯一合法动作，而不是给出"正确答案的样子"。
