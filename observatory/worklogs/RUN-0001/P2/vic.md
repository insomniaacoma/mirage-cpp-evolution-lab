# Vic Worklog — RUN-0001 P2 (WorldGraph v0 data structure, ISS-005)

- Date: 2026-09-05 · Member: vic (junior, Core/Systems pod, senior: vera) · Branch: `feature/m0-worldgraph`
- Scope note (honest): ISS-005 is owner 🎓. Orchestrator split it: I took **only the data structure + tests** against an API contract fixed by Ray (scene/mutation compile against it); projection, mutation entry, design discussion stay with owner. Contract treated as immutable.

## Current Task

`src/world_graph.h` / `src/world_graph.cpp` per contract + raylib-free `tests/test_worldgraph.cpp` + CMake wiring; MSVC build; commit.

## Current Judgment

Contract is a good M0 shape: ids + vector + append-only. My lifetime watchpoint says the expensive part wasn't code but borrow rules: `find_room()` hands out a pointer into `rooms_`; `add_room()` can reallocate and kill it. The signature can't say that, so header comments carry it. `version()` gives `build_scene` a compare-before/after escape hatch instead of holding references across mutations.

## Options Considered

- `vector<Connection>` vs per-room adjacency: vector wins at M0 — 4-6 rooms, O(E) lookup irrelevant, insertion order preserved (Dee's determinism note), trivially copyable. Adjacency buys nothing until per-vertex deletion or big graphs hurt. *Parked, not killed.*
- `RoomId=int` vs handle-with-generation: generations need things that die; rooms never die (invariant in header). Revisit when deletion semantics appear.
- Overlap validation in `add_room`: AABB is 4 comparisons (see trail) — still no: contract has no error channel (`add_room` returns `RoomId`); options left are assert (dies under NDEBUG) or silent auto-fix (worse). Data-block author owns validity.
- Next id from `rooms_.size()` vs `next_id_` member: contract lists exactly three privates; `size()` works *only* under append-only, commented in the .cpp.

## Implementation

- Edges normalized `(min,max)` so idempotence is a single-shape comparison; `connect(a,a)` no-op, `connected(a,a)` false — decided, documented, tested.
- `Connection` has no `operator==` and I can't touch the header, so `find_edge` compares fields by hand (Stuck step 3).
- Tests: `CHECK` macro (prints `#cond` + file:line, counts failures, returns 1) + `RUN_TEST`; 6 tests cover every acceptance bullet of the brief.

## Debug

- 第 9 步 **C1083** `world_graph.h: No such file or directory` from `tests/` — `src/` was never an include dir. Fixed: `target_include_directories(... PRIVATE src)` on both targets.
- 第 10 步 **C4819**（characters not representable in code page 936）: I used em-dashes in comments; MSVC on this zh-CN machine reads BOM-less UTF-8 via the ANSI code page. All code files now pure ASCII. Didn't try `/utf-8` — zero warnings without touching shared flags.
- 第 12 步 failure-path proof (self-inflicted, by design): injected `CHECK(g.version() == 42)` → printed condition + `test_worldgraph.cpp:107`, exit 1. Restored, rebuilt green. A suite whose failure mode is untested is decoration.

## Verification (real commands, real output)

```
$ "$CMAKE" -S . -B build -G "Visual Studio 17 2022" -A x64   # Configure done (4.2s)
$ "$CMAKE" --build build --config Release                     # 0 errors, 0 warnings
$ ./build/Release/test_worldgraph.exe   # 6x [ OK ] -> "all checks passed", exit 0
$ ./build/Release/mirage_m0.exe         # "MIRAGE M0" (still fine with the extra TU)
```
CMakeCache: `CMAKE_CXX_FLAGS_RELEASE:STRING=/O2 /Ob2 /DNDEBUG` — the NDEBUG evidence behind custom-CHECK. Git noted LF→CRLF on commit (known trait, docs/build.md 已知坑).

## Remaining Concerns

1. `size()`-as-next-id silently breaks (id reuse!) the day someone adds `remove_room` without revisiting; the comment is the only guard. Contract-matching vs defense-in-depth tension I can't resolve alone.
2. `rooms()`/`connections()` order is stable-by-implementation; nothing *tests* that determinism promise.
3. `version()` is `int`; if replay/snapshot ever consumes it, signed-overflow wraparound is someone's bug.

## Stuck

- 第 3 步：想 `std::find` 查 `Connection`，假设 C++20 聚合自动获得 `operator==`——**rejected by evidence**（lookup LUT-003），契约又不许改头文件 → 手写字段比较。
- 第 9 步 C1083 / 第 10 步 C4819，见 Debug。

## What I Looked Up

`observatory/learning/RUN-0001/vic/lookup-001.md`：assert×NDEBUG（cppreference + 本机 CMakeCache 实测）、vector 失效规则（cppreference push_back）、C++20 聚合比较（cppreference default_comparisons）、AABB 重叠测试（MDN / LearnOpenGL，**adopted: no**，理由是 API 形状不是成本）。

## What I Still Don't Get

- 编译器对"指针指向 vector 元素"毫无防线——除了头注释 + review，M0 有没有机械化的办法？（sanitizers 是事后抓不是防。）
- C4819 为什么只是警告：MSVC 到底按 936 解码了那些 UTF-8 字节还是直接丢弃？`/utf-8` 是不是这台机器所有 target 的正确默认？没敢动共享 flags。
- 边的 `(min,max)` 规范化是我自己定的（契约没说）。若 Nico 的 mutation 将来要区分方向，它就是错的；我只有 ISS-005"门洞即连接的存在/不存在"一句支撑，没找到无向性的书面出处。
