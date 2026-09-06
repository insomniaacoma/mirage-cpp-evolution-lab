# ISS-019: RFC-0005 + 音频模块出生（氛围循环 + 突变音频 cue）

| 字段 | 值 |
|---|---|
| Milestone | M1 |
| Type | feature（新模块出生，T2） |
| Owner | kai/kit（🎓Hook 合法形态） |
| Reviewer | Vera（realtime 边界）+ Arden（模块边界） |
| Status | open |
| Depends on | — |

## 背景

Kai 从 M0 起等的出生信号到了（R-001 判定）。charter §4 的音频禁令由 Owner M1 裁决解除——**但 realtime 边界红线从第一行代码生效**：gameplay 不进 audio thread、audio thread 不被无界等待/持锁阻塞。

## 需求 / 验收标准

- [ ] RFC-0005（Kai 主笔）：音频模块最小架构——用什么库、线程模型、Music Clock 是否本轮引入、与 gameplay 的交互协议（倾向：gameplay 发命令进队列，audio thread 消费；禁止反向回调）。
- [ ] 交付：进入游戏有氛围音乐循环（可静音）；突变瞬间有音频 cue（与视觉 cue 同帧发出、异线程播放）。
- [ ] Beat/Phrase 概念**只允许**以最简形式存在（如"音乐在 x 秒处循环"），不接玩法。
- [ ] 模块注册：`module.registered` audio，owner = kai。
- [ ] Vera 的 review 必含：线程边界图（谁在哪条线程、锁在哪、无界等待在哪被禁止）。

## Non-goals

Beat Event 驱动 WorldGraph（M2+ 候选）、DSP、3D 音频、音频资产管线（循环文件手放 assets/）。
