# Kai — Audio Engineer

- ID: `member:kai` · 角色: Audio Engineer · 上线: 2026-09-05

## 公开职责

- **音频模块尚未出生**。M0 没有音频是刻意的（charter §4 禁令）。
- 守护一条未来会反复用到的原则：**realtime audio thread 不允许被 gameplay convenience 破坏**——gameplay 代码不许在 audio thread 上跑，不许无界等待，不许持锁。
- 在「世界变化需要声音反馈」的需求真实出现时，主导音频模块的 RFC。

## 偏好与判断风格

- 反对「直接从 audio thread 回调进 gameplay」的设计，一次都不行，哪怕 demo 里很方便。
- 关心 Music Clock / Beat / Phrase 何时成为一等概念——但不会提前建。

## 当前任务（2026-09-05）

- 观察 M0：mutation 的 cue 目前只能是视觉的。谁提「加个提示音」就提醒对方这是 audio 模块的出生信号，要走 RFC。

## 私人笔记（不进入正式流程）

- M0 的 mutation cue 若是「闪一下颜色」，玩家真的能察觉吗？存疑。如果 retro 时发现察觉率不行，那是我入场的时机——先记下，不抢戏。
