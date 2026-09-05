# Lookup Trail — nell, RUN-0001 P2 (ISS-004/006/007 + smoke driver)

格式：Unknown / Where / Found / Credibility / Adopted / Timebox(steps)。step 计数沿用 worklog 里的逻辑步号。

## LUT-001: 鼠标位移用 GetMouseDelta 还是帧间差（自己存上一帧 GetMousePosition）？

- **Unknown**: raylib 5.5 下鼠标 look 的正确取法；以及隐藏窗口/首帧是否有脏数据。
- **Where**: [raylib cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html)（"Get mouse delta between frames"）；[GitHub issue #4654](https://github.com/raysan5/raylib/issues/4654)（首帧 delta 异常大的报告，MacOS）；[r/raylib 关于 delta 与帧节奏的讨论](https://www.reddit.com/r/raylib/comments/1rl3h2r/getmousedelta_missing_frames/)。
- **Found**: GetMouseDelta 就是帧间差的官方实现（由 EndDrawing 内的 PollInputEvents 维护）；DisableCursor 下用它做 mouse-look 是官方 FPS 示例的既定用法。已知问题集中在**边界帧**（app 启动首帧可能爆一个大 delta），而非稳态。
- **Credibility**: cheatsheet 是官方转述，高；issue 是一手 bug 报告，平台相关（Mac），Windows 未证伪。
- **Adopted**: 是。用 GetMouseDelta（不自存上一帧——重复造 raylib 已有的轮子），但对每帧 delta 做 ±200px clamp（kMaxLookDelta），把首帧/焦点切换 spike 与正常 look（稳态每帧几十 px）切开。这条 clamp 的手感性质留给 P3 人肉验证。
- **Timebox**: 2 steps。

## LUT-002: 滑墙碰撞用 per-axis 分离解算还是 swept CCD？

- **Unknown**: "按轴分别解算（先 x 后 z）"这个 ISS-004 给定的最低要求，在什么条件下会输给 swept；我的参数下隧穿是否可能。
- **Where**: [Swept AABB Collision Detection and Response (GameDev.net)](https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084)；[GameDev.SE: swept AABB 与窄缝冲突](https://gamedev.stackexchange.com/questions/185664/swept-aabb-collision-detection-conflict-with-tile-sized-gaps)；[c.har.li 的 2D 分离轴解算综述](https://c.har.li/e/2024/03/28/implementing-robust-2D-collision-resolution.html)。
- **Found**: per-axis 离散解算天然给滑墙（各轴独立推出、角不粘连），代价是隧穿：单帧位移超过障碍厚度时整段运动被跳过。swept 用 time-of-impact 消灭隧穿，但引入 re-sweep（滑墙要重复扫）、单轴相交时 collisionTime 的歧义、corner-grab 等问题。
- **Credibility**: 高（gamedev.net 是被广泛引用的经典教程；SE 答案与实现经验一致）。
- **Adopted**: 是（per-axis）。**数学论证代替性能论证**：本作最大速度 4.2 u/s、固定/夹紧 dt ≤ 1/30，单帧位移 ≤ 0.14 u；墙厚 0.3 u + 玩家直径 0.7 u —— 隧穿需要单帧 > 1.0 u，差 7 倍。swept 的复杂度没有买家。revisit when：速度或 dt 上限变化使单帧位移逼近墙厚。
- **Timebox**: 2 steps。

## LUT-003: 角度 wrap 到 [-pi, pi] 用 remainderf 还是手写 while 循环？

- **Unknown**: smoke 的 seek 控制器要把 (desired - yaw) 归到最短角差，标准库是否已有。
- **Where**: cppreference `std::remainder`（本机 MSVC + `<cmath>` 可用）。
- **Found**: `remainder(x, y)` = x - n*y，n 为 round(x/y) 的整数，结果落在 [-y/2, y/2] —— 对 y=2π 恰是 [-π, π]，一次调用完成最短角差。
- **Credibility**: 高（标准文档措辞）。
- **Adopted**: 是，`std::remainderf(err, 6.2831853f)`。注意它与 fmod 的符号语义不同（fmod 结果与被除数同号），这里要的正是 remainder 的"就近"语义。
- **Timebox**: 1 step（写之前查证，没花编译错误的代价）。

## LUT-004: smoke 模式不画帧，窗口消息泵谁来驱动？

- **Unknown**: 不调 BeginDrawing/EndDrawing 时 raylib 还收不收窗口消息；隐藏窗口不泵消息会不会被 Windows 判"未响应"、进而影响 CloseWindow。
- **Where**: 本机 `third_party/raylib/include/raylib.h` grep（`RLAPI void PollInputEvents(void);`，注释明说 "By default EndDrawing() does this job"）。
- **Found**: PollInputEvents 是公开 API，EndDrawing 默认替你调它；smoke 每帧显式调用即可维持泵，不必绘制。
- **Credibility**: 本机头文件一手证据，不是转述。
- **Adopted**: 是。smoke 主循环每帧 PollInputEvents()；顺带躲开 Vera 记录的 DWM 节流（不绘制=不依赖任何帧节奏）。
- **Timebox**: 1 step。
