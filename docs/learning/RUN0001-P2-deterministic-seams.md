# Learning Hook — 确定性接缝：InputFrame 与浮点尘埃（RUN-0001 · P2 · guided）

> 来源：nell 的 gameplay 实现（ISS-004/006/007）+ squeeze 探针。Guess 模式首选材料：
> 关键节点先自己答，再对照 nell 的真实做法。

## 当时面临的问题

两件看似无关的事，其实是同一个设计问题——**怎么让"人的输入"和"脚本的输入"走同一条路**，以及**当确定性被 9e-8 打破时听谁的**。

## 关键决策链

1. **InputFrame 是唯一的输入单位**
   - 真人路径：raylib 键鼠 → 填 `InputFrame` → `player.update(input, dt, solids)`。
   - smoke 路径：脚本逐帧填**同一个结构体** → 同一个 update。
   - 为什么重要：smoke 的 PASS 才能证明"真人也会走到出口"——仿真和现实共享物理。任何为 smoke 开的后门（直接改坐标）都会让验收说谎。
2. **固定 dt 而不是 GetFrameTime**
   - 教训链：隐藏窗口被 DWM 节流（REVIEW-0001 发现）→ smoke 一旦依赖墙钟节奏，帧数就不可复现 → 固定 dt=1/60 + 语义断言（fire 时连续停留 <4s，而不是"第 498 帧"）。
   - nell 的错误答案（Guess 对比点）：他最初用帧号断言，排期漏算"进门走到房心"的累计尾巴后失败——**确定性不该绑在排期巧合上**，这是他从帧号改成语义判别的真实过程。
3. **浮点尘埃 vs 探针**
   - squeeze 探针抓到：`8.0f - 0.35f` 的舍入让"解算后的位置"距墙内侧 ~9e-8——严格穿透检查从此每帧报警。
   - 两种修法：放宽检查（掩盖）/ 引入 `kCollisionSkin=1e-4` 容差（承认碰撞半径是模拟不是数学）。
   - nell 选了后者并保留探针作回归——**修数值语义，不修证据**。

## 关键代码

```cpp
struct InputFrame {          // DEC-003: human and script both fill this
    float move_forward, move_strafe, look_dx, look_dy;
};
void Player::update(const InputFrame& input, float dt,
                    const std::vector<SceneBox>& solids); // per-axis circle-vs-AABB
```

## 一个替代方案（被否决）

swept AABB（连续碰撞检测）：对窄缝冲突更稳，但 M0 的移动速度（4.2 u/s）远低于每帧穿透阈值，per-axis + skin 够用——查证记录在 nell 的 lookup-001（gamedev.SE 窄缝冲突案例）。

## Review 问题（自测）

1. 如果把鼠标灵敏度调高 10 倍，InputFrame 契约需要变吗？谁吸收这个变化？
2. `kCollisionSkin` 为什么不改成"把墙 AABB 缩小 1e-4"？（提示：谁拥有墙？谁拥有玩家？）
3. smoke 断言为什么应该写"fire 时连续停留 <4s"而不是"frame==498"？

## 建议 Owner 亲手做的小练习

把 `--smoke` 的脚本输入改一个参数（比如 B 房停留时长改 3s），预测 smoke 会 FAIL 在哪一句，
然后跑一遍验证你的预测。再解释为什么改动后 `graph version 9 -> 11` 不会变。

## 术语卡（guided）

- **接缝（seam）**：人为设计的可替换边界——InputFrame 让"输入来源"可替换而不动游戏逻辑。
- **确定性**：同输入必同输出。M0 的三层保障：固定 dt、无随机、语义断言。
- **浮点尘埃**：实数运算的舍入残差在严格比较下暴露为"假阳性违规"。
