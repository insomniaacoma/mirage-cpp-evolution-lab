# Build（M0）

## 前置条件

- **VS 2022 Community**（含 "Desktop development with C++"）→ 本机安装于 `E:\Microsoft Visual Studio\2022\Community`
- CMake：用 VS 自带的（无需单独安装）：
  `E:\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`
  （下文用 `%CMAKE%` 指代；Git Bash 下可直接把它 alias 进 PATH）
- Python 3.x（仅 observatory 工具需要）

## 首次准备

```bash
# 1) 获取 raylib 5.5（MSVC 预编译，ADR-0002）。直连 GitHub 失败时走代理：
python tools/fetch_raylib.py --proxy http://127.0.0.1:7897
# 成功标志: third_party/raylib/lib/raylib.lib 存在（该目录 gitignored，可随时重建）
```

## 构建 & 运行（Git Bash 示例）

```bash
CMAKE="/e/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
"$CMAKE" -S . -B build -G "Visual Studio 17 2022" -A x64
"$CMAKE" --build build --config Release
./build/Release/mirage_m0.exe          # 窗口版
./build/Release/mirage_m0.exe --smoke  # 无头确定性冒烟（P2 起可用）
./build/Release/test_worldgraph.exe    # 数据结构测试（P2 起可用）
```

## 观测站（OW0）

```bash
python tools/obslog.py check   # 校验事件日志
python tools/obslog.py view    # 生成 observatory/view/data.js
# 然后直接用浏览器打开 observatory/view/index.html（file:// 即可）
```

## 已知坑

- 直连 github.com 会 000/301：走本地代理 `127.0.0.1:7897`（SD-2）。
- 仓库全局 `core.autocrlf=true`：工作区 CRLF、库内 LF。observatory 校验器按行 strip `\r`（DEC-RUN0001-P1-002）。
- raylib 静态链接需附系统库：`opengl32 gdi32 winmm user32 shell32 imm32 setupapi`（.spike 已验证的链接组合）。
