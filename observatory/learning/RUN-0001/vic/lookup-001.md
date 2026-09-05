# Lookup Trail — vic, RUN-0001 P2 (ISS-005 WorldGraph v0)

格式：Unknown / Where / Found / Credibility / Adopted / Timebox(steps)。step 计数沿用 worklog 里的逻辑步号。

## LUT-001: Release 构建里 `<cassert>` 的 assert 还活着吗？（决定测试宏取舍）

- **Unknown**: runbook 用 `--config Release` 构建；若 Release 定义 NDEBUG，assert 会被编译掉，测试变摆设。
- **Where**: cppreference [assert](https://en.cppreference.com/w/cpp/error/assert)（前两个 URL 猜错 404，第三次才对——路径是 `/w/cpp/error/assert`）；本机 `build/CMakeCache.txt`。
- **Found**: NDEBUG 在 `<cassert>` 被 include 处已定义时，`assert` 展开为 `((void)0)`，条件**不求值**（until C++26）。本机实测：`CMAKE_CXX_FLAGS_RELEASE:STRING=/O2 /Ob2 /DNDEBUG`。
- **Credibility**: cppreference 对标准措辞的转述，高；CMakeCache 是本机直接证据，不是转述。
- **Adopted**: 是。测试用自定义 `CHECK` 宏（打印 `#cond` + file:line，失败计数，main 返回 1），不用 `<cassert>`。
- **Timebox**: 2 steps（查证 1 + 本机 grep 1）。

## LUT-002: `std::vector::push_back` 什么时候让 `find_room` 返回的指针悬垂？

- **Unknown**: add_room 后，之前拿到的 `const Room*` / `rooms()` 引用是否还活着。
- **Where**: cppreference [vector/push_back](https://en.cppreference.com/w/cpp/container/vector/push_back)。
- **Found**: "If after the operation the new size() is greater than old capacity() a reallocation takes place, in which case all iterators (including the end() iterator) and all references to the elements are invalidated. Otherwise only the end() iterator is invalidated."——即只有重分配才杀引用，但 M0 契约层面我无法承诺容量。
- **Credibility**: 高（cppreference，措辞直接引用）。
- **Adopted**: 部分。**不**做 `reserve()`——它只缓解个别调用点，改变不了 API 借出语义；把失效规则写进 `world_graph.h` 头注释（"views into rooms_; any add_room() may reallocate"），并给 `version()` 注明用途（compare-before/after，别跨 mutation 持引用）。
- **Timebox**: 1 step。

## LUT-003: C++20 聚合体自动获得 `operator==` 吗？（我走错方向的假设）

- **Unknown**: 想对 `Connection{a,b}` 直接 `std::find`，假设 C++20 会为聚合隐式合成 `operator==`。
- **Where**: cppreference [default_comparisons](https://en.cppreference.com/w/cpp/language/default_comparisons)。
- **Found**: 不会。只有显式 `= default` 的 `operator==`，或 defaulted `operator<=>`（它隐式带一个 `operator==`）才生成；页面原例：无任何比较声明的聚合 `p == q` 是编译错误。**Rejected by evidence**：我的初始假设错了，且头文件契约不许加 `= default`，于是 `find_edge` 手写字段比较。
- **Credibility**: 高。
- **Adopted**: 是（改实现，不是改假设的来源）。
- **Timebox**: 1 step（写之前犹豫查证，没花编译错误的代价）。

## LUT-004: add_room 要不要做房间矩形重叠校验？

- **Unknown**: M0 数据块手写 4-6 个房间，插入时校验重叠是否值得。
- **Where**: [MDN 3D collision detection](https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection)、[LearnOpenGL 2D AABB](https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-Detection)。
- **Found**: AABB 重叠 = 每轴区间比较（min-corner + extents 表示下共 4 次比较），成本可忽略；难的不是算法是拒绝路径。
- **Credibility**: 中高（游戏开发常用参考，非标准文档；算法本身无争议）。
- **Adopted**: **否**。理由是 API 形状而非成本：契约的 `add_room` 返回 `RoomId`、无错误通道；只能 assert（Release 下消失，见 LUT-001）或静默修正（更糟）。校验留给将来真实的坏数据出现时，且那时应该有 owner 参与的错误处理决策。
- **Timebox**: 2 steps。
