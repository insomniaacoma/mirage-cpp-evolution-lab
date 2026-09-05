# Nell — P2 Worklog（ISS-004 控制器 + ISS-006 突变 + ISS-007 出口 + smoke driver）

- Date: 2026-09-05 · Member: nell (junior, Gameplay pod, senior: nico) · Branch: `feature/m0-play`
- Scope: ISS-004 / ISS-006 / ISS-007 的代码 + DEC-RUN0001-P1-003 的 smoke driver。消费 world_graph/scene，未改其一行。

## Current Task

`src/player.{h,cpp}`（InputFrame + 圆 vs AABB 按轴解算）、`src/mutation.{h,cpp}`（确定性触发 + cue）、`src/exit.{h,cpp}`（双因子门控）、main.cpp 接线（真人模式 + 固定 dt smoke driver，替换 600 帧守卫与轨道相机）。

## Current Judgment

M0 控制器该是"一个结构体 + 一个 update"：状态只有 x/z/yaw/pitch，碰撞就是每帧对全量 solids 做两遍（x 一遍 z 一遍）的圆-矩形测试。相机派生放 **main.cpp**：`Camera3D` 是 raylib 类型，ADR-0002 把 raylib call surface 锁在 main.cpp/scene.cpp；`kEyeHeight` 从 player.h 导出，因为眼高是**身体数据**而相机数学是 main 的事。player.h include scene.h 只是消费 `SceneBox` 数据（无任何 raylib 函数调用）——这是我对 ADR 条文的解释，已记录，若 Rin/Arden 认为include 也算 surface，改起来是一行。

## Options Considered

- **突变语义：累计 vs 连续**——选**累计**（dt 累加，进出去不留、回来接着攒）。理由写进了 mutation.cpp 注释："这地方不对劲的感觉攒够了"比"必须连续罚站 5 秒"贴主题。smoke 用 excursion 覆盖：脚本离开 B 再回来，fire 发生在回程后 <4s 连续停留时（断言 `stay_frames*dt >= 4.0` 即 FAIL），连续语义下这不可能通过。
- **触发参数硬编码在哪**——不在 mutation.cpp 里写死 id，而是 `MutationSpec{...}` 由 main 在 `build_demo_world()` 旁边接线（DemoWorld 结构把 room id 随 graph 一起带出来）。规则硬编码在 update()（Nico 的疼痛点 P3 预测不变），但**数据**只有一个出处，id 漂移无从发生。
- **room_at 谓词**——按指示放 mutation.cpp 本地；exit.cpp 需要同款判定、smoke 需要第三份。三份 6 行重复 + smoke 里用交叉断言兜底（exit_reached 与脚本自己的判定不一致即 FAIL）。第三份出现时再谈工具库，现在提前抽象是替不存在的压力付钱。
- **碰撞解算：per-axis 离散 vs swept CCD**——per-axis。数学论证：最大位移 4.2 u/s ÷ 60 = 0.07 u/帧，墙厚 0.3 u，隧穿需要单帧位移 > 墙厚+直径，差一个数量级。swept 的 re-sweep / corner-grab 复杂度 M0 没有买家（lookup LUT-002）。
- **对角线速度**——归一化（合成向量 len>1 时除以 len）。Nico 大概会说"不管它"，但 41% 的斜向加速是能被手感感知的 bug，两行代码的事。
- **smoke 是否画帧**——不画。Vera 的 DWM 发现（REVIEW-0001）说明隐藏窗口连 BeginDrawing 的节奏都不能信；smoke 每帧调公开的 `PollInputEvents()` 泵消息（raylib.h 里确认是 RLAPI），仿真路径与真人共用，只是渲染被跳过——这个取舍记在这里，Dee 若认为 smoke 必须覆盖 draw 路径，需要重新讨论。
- **脚本输入走后门 vs 走 InputFrame**——seek 控制器只填 InputFrame：转向量算成弧度后**除以 kMouseSensitivity 换算成"像素"**再塞进 look_dx，与真人共用同一条灵敏度路径。直接写 player.yaw 被拒绝（DEC-003 的接缝就是为了堵这个）。

## Implementation

- player.h：`InputFrame`（DEC-003 的输入单元，NSDMI 全零）、`Player{x,z,yaw,pitch}`、六个有名手感常量（kWalkSpeed=4.2、kMouseSensitivity=0.0025、kPlayerRadius=0.35、kBodyHeight=1.8、kEyeHeight=1.7、kPitchLimitDeg=89）、`player_penetrates()`（smoke 断言用，非玩法接口）。yaw 约定：0 朝 -Z，正 yaw 向右转；forward=(sin yaw, -cos yaw)，right=(cos yaw, sin yaw)。碰撞前先做**竖直跨度门控**（box y 区间与 [0,1.8] 相交才算墙）——否则每个门洞上的 lintel（y∈[2.6,3.2]）会把整张地图封死。
- mutation.h/cpp：`MutationSpec` + `Mutation::update(graph, player, dt)`。fire 时走 ISS-005 的 `disconnect/connect` 入口（不绕投影），cue 计时器（0.6s flash / 3s caption）在模块内倒数，main 渲染时读 `cue()`。
- exit.h/cpp：`exit_reached(graph, player, exit_room, mutation_applied)` 双因子。拓扑因子在当前数据下冗余，但它在世界数据被人改动时保住门控的诚实性。
- main.cpp：DemoWorld 结构携带 room ids；真人循环（DisableCursor + GetMouseDelta clamp ±200px——首帧 spike 有前科，见 LUT-001）；smoke 相位机（SEEK_B1→DWELL1→SEEK_A→SEEK_B2→DWELL2→SETTLE→SEEK_D→SEEK_E→PASS_HOLD），waypoint 全部是房心、天然对齐门洞中线；断言：fire 恰一次、版本 +2、累计语义判别、E 突变前不可达、每帧无穿透、exit_reached 与脚本几何交叉验证。房间在 smoke 开头**拷贝**出来（world_graph.h 借出规则要求的动作，虽然运行时没有 add_room）。

## Debug

- **squeeze 探针抓到真 bug（本日最有价值 10 分钟）**：scratch 探针模拟"玩家站在 C-D 门洞里被突变封死"，probe A（C 侧，spawn x=8.05）三项 CHECK FAILED——`push_out` 把玩家推到 `lo - r`，但 `8.0f - 0.35f` 的浮点舍入落在真实值**内侧** ~9e-8，严格 closest-point 判定从此每帧都报穿透（探针 final x=7.6500 复现）。probe B（高侧）因舍入方向相反侥幸通过——**rejected by evidence**：我原本假设"resolve 后的位置必然无重叠"。修复：`kCollisionSkin = 1e-4`，静止位置离墙面一个 skin 宽，解算收敛、判定与解算自洽。修复后 SQUEEZE: PASS（16 checks）。
- **smoke 相位预算第一次排错**（设计期，没花编译周期）：最初 dwell1=3.0s 的排期没算"进门后走到房心还有 5u 路"这段 in-B 尾巴的累计，dwell 会在 SEEK 阶段意外攒满 5s。修法不是调数字，而是**把断言从帧号改成语义**（fire 时连续停留必须 <4s）——对 seek 尾巴的时长波动鲁棒，对"连续 vs 累计"的判别边界依然干净（连续语义预测 fire 时连续停留=5.0s）。
- **探针的 tie 位**：精确墙中线（x=8.15）两侧距离浮点相等，tie 断向低侧是代码保证，但 spawn 点本身落不落得进 tie 是 float 脆弱态——探针改用 8.05/8.25 偏置，tie 语义留给代码注释。
- main.cpp 写过 `mut.update(world=demo.graph, ...)`（非法语法）和 `const DemoWorld`（传不出 `WorldGraph&`），编译前自查修掉，没花编译错误。

## Verification (real commands, real output)

```
$ cmake -S . -B build -G "Visual Studio 17 2022" -A x64   # Configure done
$ cmake --build build --config Release                    # 0 errors, 0 warnings
$ ./build/Release/mirage_m0.exe --smoke                   # x5 次，输出逐字节一致：
SMOKE: mutation applied at frame 498 (graph version 9 -> 11)
SMOKE: exit reached at frame 809
SMOKE: PASS                                               # exit 0，wall ~0.6s
$ ./build/Release/test_worldgraph.exe                     # 6x [ OK ]，all checks passed，exit 0
$ build/scratch/out/Release/squeeze_scratch.exe           # SQUEEZE: PASS (16 checks, 0 failures)
$ timeout 3 ./build/Release/mirage_m0.exe                 # exit 124（窗口模式正常起跑，被 kill）
$ grep -rP '[^\x00-\x7F]' src/*.h src/*.cpp CMakeLists.txt # 我名下文件 ASCII clean
```

## Remaining Concerns（含待 P3 Dee 人肉 checklist）

人肉路径无法自动验证，以下留给 Dee 的 P3 checklist：
1. **手感参数是纸面推断**：走速 4.2 u/s、灵敏度 0.0025 rad/px、fovy 70——真人跑一圈，尤其灵敏度（raylib 的 mouse delta 无加速曲线，可能偏快/偏慢）。
2. **ESC 退出 + 鼠标释放路径**（Vera 的 watchpoint）：M0 无 pause 态，鼠标靠 CloseWindow 隐式释放；确认 ESC 真能退出、退出后系统光标恢复。
3. **GetMouseDelta 首帧 spike**：已 clamp ±200px，但要在真机看第一帧有没有甩头（GitHub #4654 是 MacOS 报告，Windows 未证伪）。
4. **cue 可察觉性**：0.6s 全屏淡紫闪 + 3s 渐隐文案——Kai 的 P4 预测（视觉 cue 察觉率）在这份实现上待验证。
5. **win 后行为**：LAYER 01 - CLEARED 常驻 + 可继续走动 + ESC 退出（issue 里的 em-dash 因 MSVC 936 代码页改为 ASCII hyphen，语义不变）。
6. 突变瞬间如果玩家**贴着**将被封死的 C-D 门洞站立（不在门洞内但距离 < r+skin）：cue 会闪但几何只差几厘米——人肉看看观感是否"说谎说得不够狠"。

## Stuck

- 第 9 步（squeeze 探针首跑 FAIL→定位→修复→复跑）：约 2 logical steps。假设被证据推翻的细节见 Debug。
- 第 6 步（smoke 相位排期重设计）：1 step，见 Debug 第二条。
- 其余步骤（读契约/设计/实现/构建/smoke 首跑 PASS）未卡：构建一次过是 raylib 预编译直接可拷 + 前任把坑都写在 build.md 和 vera/vic worklog 里的功劳。

## What I Looked Up

`observatory/learning/RUN-0001/nell/lookup-001.md`：GetMouseDelta 语义与首帧 spike（raylib cheatsheet + GitHub #4654）、per-axis vs swept CCD（gamedev.net swept 教程 + 数学论证）、remainderf 角度 wrap（cppreference）、PollInputEvents 公开性（本机 raylib.h grep）。

## What I Still Don't Get

1. squeeze 的 nearer-face 推出在玩家中心恰好过墙中线时会把人"送过去"目的地侧——物理上自洽，但游戏叙事上"世界把你吐到它想去的地方"和"穿墙 bug"的界线在哪，我没有判据，已提给 Nico（mentoring/nell-nico.md）。
2. InputFrame 的 look 用像素做单位、灵敏度常量在 player 侧——smoke 脚本想要"每帧转 0.067 rad"就得除以灵敏度再塞回去，**单位耦合方向是反的**。如果 InputFrame 注定长成 replay 单元（DEC-003 revisit when），look 该从一开始就是弧度吗？可我还是觉得"原始设备增量"才是诚实的输入……
3. 三份 point-in-room 谓词靠 smoke 交叉断言兜底——"第三份出现才值得抽象"是 charter P1 的节奏，但有没有比等第三份更早的机械信号（比如断言先于抽象亮起来）？
