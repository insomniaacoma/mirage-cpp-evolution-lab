# DEC-RUN0001-P1-002 · 事件日志行尾策略：写如所写，读时容 \r

- Level: Decision Capsule
- Phase: P1 · Owner: Mora · Reviewer: Dee

## Problem
本机 git 全局 `core.autocrlf=true`：工作区文件 CRLF、库内 LF（P0 提交时出现 LF→CRLF 警告）。JSONL 每行末尾的 `\r` 对 JSON 合法（JSON 允许 \r 作空白），但逐行解析器若不处理会把 `\r` 粘进行内容。

## Options
A) 强制全仓 LF（.gitattributes `* text=auto eol=lf`）— 动全仓，超出本决策影响面。
B) 写入方不管，读取方按行 strip 尾部 `\r`（选定）。
C) 校验器报 error — 会让 Windows 用户每次提交后误报。

## Choice & Reason
B。证据：seed 日志 30 行在 CRLF 下经 `json.loads` 全部通过（\r 是 JSON 合法空白）。`obslog check` 将实现 strip-\r。

## Consequence
C++ 端未来读日志也要 strip \r（记入 data-model 备注）；revisit when：出现跨平台二进制 payload。

## Revisit When
引入正式 .gitattributes 策略时（ Owner 偏好问题，留给 Owner）。
