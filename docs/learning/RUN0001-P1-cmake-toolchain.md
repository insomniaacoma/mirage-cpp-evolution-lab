# Learning Hook — CMake 工具链与 MSVC（ISS-001 · RUN-0001 · P1 · guided）

> 原 🎓 任务，依 RUN-0001 协议 §4.8 由 Agent 实现，本 Hook 保留 Owner 亲手重做的入口。

## 当时面临的问题

仓库没有任何构建系统；M0 一切代码都要从"cl.exe 怎么被找到"开始。额外约束：GitHub 直连不稳（SD-2），raylib 只能走代理拿预编译包。

## 关键代码/配置

- 根 `CMakeLists.txt`：单 target `mirage_m0`，C++20，`CMAKE_CXX_EXTENSIONS OFF`（强制严格 `std::` 习惯）。
- `.spike/`：5 帧隐藏窗口的最小 raylib 程序 + 链接组合 `raylib opengl32 gdi32 winmm user32 shell32 imm32 setupapi` —— 用 30 行代码把"预编译库 ABI 是否兼容"这个最大未知先消灭。
- `tools/fetch_raylib.py`：版本钉死 5.5，解压后校验 `lib/raylib.lib` + `include/raylib.h` 存在，幂等（marker 文件）。

## 为什么这样设计

- 用 VS 自带 CMake 而非单独安装：少一个版本漂移源；`-G "Visual Studio 17 2022"` 让 MSBuild 自己处理 vcvars 环境，不需要在 Git Bash 里 source vcvars64.bat。
- spike 放 `.spike/`（gitignored）：证据属于决策过程，不属于仓库资产。
- `third_party/` gitignored + fetch 脚本：仓库不背二进制，但任何人 5 分钟可重建。

## 一个替代方案

MinGW gcc 8.1.0（也在机器上）：raylib 有 mingw 预编译包，命令行体验更顺。没选它因为 Owner 的 VS 已装好、调试器体验（Visual Studio）更好，且未来 C++20/23 特性 MSVC 跟进更快。

## Review 问题（自测）

1. 为什么 `-A x64` 显式指定？默认平台取决于什么？
2. `CMAKE_CXX_EXTENSIONS OFF` 会改变哪类编译失败？
3. fetch 脚本的幂等性靠什么实现？如果 raylib 出 5.6，脚本要改几处？

## 建议 Owner 亲手做的小练习

把 `.spike/` 删掉，仅凭 `docs/build.md` 从零重建 spike 并跑通——期间只允许查 raylib 官方 examples，不许看 git 历史。这会逼你独立面对 include/lib/系统库三件套。

## 勘误记录

无。
