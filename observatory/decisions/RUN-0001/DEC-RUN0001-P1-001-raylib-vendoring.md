# DEC-RUN0001-P1-001 · raylib 以 fetch 脚本 vendoring，不入库二进制

- Level: Decision Capsule（单模块内、可逆）
- Phase: P1 · Owner: Vera · Reviewer: Arden（随 ISS-001 review）

## Problem
raylib 预编译包 ~2.4MB 压缩 / ~9MB 解压。提交二进制进 git 让仓库永久变重；不提交则新人无法开箱构建。

## Options
A) 提交 third_party/ 进仓库 — 简单但仓库永久变重，且违反"仓库只放可审阅文本"的直觉。
B) git fetch 脚本 + gitignore（选定）— 可复现、仓库干净。
C) git submodule 指向 raylib 源码 — 引入源码构建链路，M0 不需要。

## Choice & Reason
B。证据：fetch 走代理实测 206 成功；脚本幂等（marker 文件）。

## Consequence
CI/离线机器需要先跑 fetch（docs/build.md 已写明）；revisit when：出现无网构建需求。

## Revisit When
raylib 升级（5.6+）或出现离线 CI。
