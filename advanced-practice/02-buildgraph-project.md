# 进阶实战 2：BuildGraph 生成项目

示例项目：`examples/build_host/`（`build.eidos` + `src/main.eidos`，`verify-examples.ps1` 独立验证）

本项目演示**能力约束的构建系统**（[Build host 与 BuildGraph](../advanced/07-build-host.md)）：构建程序在编译期投影能力、声明 BuildGraph，编译器负责验证与执行。

## 项目结构

```
examples/build_host/
├── eidos.toml      # manifestSchema 3 + [build] 段
├── build.eidos     # 构建程序：投影能力、声明 BuildGraph
└── src/main.eidos  # 普通源码（无构建相关代码）
```

## manifest `[build]` 段

```toml
manifestSchema = 3

[language]
version = "0.8.0-alpha.1"

[package]
name = "dev.eidos.tutorial.build-host"
version = "0.1.0"

[dependencies]
std = "0.1.0-alpha.1"

[build]
program = "build.eidos"
outputRoots = ["build/generated"]
```

`program` 指定构建程序；构建程序与输出必须在 project root 内，program 和文件输入不得与 output root 重叠。

## 构建程序

```eidos
Session :: comptime build.session();
Host :: comptime build.host(Session);
Target :: comptime build.target(Session);
Emit :: comptime build.emit(Session);
BuildGraph :: comptime build.graph(Emit, [], []);
```

构建程序先取得 opaque session，再投影分离的 `build.Process` 与 `build.Emit` 能力，最后返回唯一的顶层 `BuildGraph`。空命令列表的空 graph 是合法的起点——逐步添加 `build.command`（登记工具、输入、输出）与 `build.generated_source`（生成源码，目录自动加入 import roots）即可扩展为真实生成管线。

## 验证

```powershell
# 教程 CI 使用的方式（no-cache 强制重跑）
dotnet run --project src/Eidosc/Eidosc.Cli -- build --project examples/build_host --target typed --no-cache --deny style --emit-build-graph build/graph.json --no-color

# 观察 capability 访问与 step/cache 状态
dotnet run --project src/Eidosc/Eidosc.Cli -- build --project examples/build_host --target-name main --trace-build
```

`verify-examples.ps1` 对 `examples/build_host` 执行构建并校验产物存在（见 [示例验证与测试](../toolchain/02-verification-and-testing.md)）。

## 练习

1. 在 `[build]` 段登记一个环境变量与一个文件输入，观察 cache key 变化（`--trace-build`）；
2. 添加一个 `build.command` 步骤（可先用手边的可执行文件），声明输出后确认"output 已实际生成"校验；
3. 把生成结果改为 `generated_source` 并让 `src/main.eidos` 引用它（提示：[编译期元编程](../advanced/06-metaprogramming.md) 的 typed protocol 与 provenance）。
