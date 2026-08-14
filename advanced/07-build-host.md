# Build host 与 BuildGraph

示例文件：`examples/build_host/`（`build.eidos` + `src/main.eidos`，独立项目验证）

## 能力约束的构建域

`build` 与 `meta` 一样是无需 import 的编译器内建域，但只能在 `[build].program` 指定的构建程序中取得能力。普通 pure comptime 若尝试取得文件、环境、进程、网络或 artifact emit 能力会被拒绝：

```toml
[build]
program = "build.eidos"
fileInputs = ["schema/model.json", "assets"]
environment = ["SDK_ROOT"]
outputRoots = ["build/generated"]

[[build.tools]]
name = "generator"
path = "tools/generator.exe"
```

- `fileInputs` 可以声明文件或目录；目录会递归展开并按项目相对路径稳定排序；
- 所有输入内容、登记环境变量的存在性和值、登记工具的可执行文件路径/hash、build program、host triple 与 target triple 都进入 Build host cache key；
- build program、文件输入与输出必须留在 project root 内，program 和文件输入都不得与 output root 重叠；
- 工具必须用显式路径登记，不依赖偶然的 `PATH` 查找。

## BuildGraph 投影

构建程序先取得一个 opaque session，再投影分离的 `build.Process` 与 `build.Emit` 能力，并返回唯一的顶层 `BuildGraph`：

```eidos
Session :: comptime build.session();
Process :: comptime build.process(Session);
Emit :: comptime build.emit(Session);

Generate :: comptime build.command(
    Process,
    "generate",
    "generator",
    ["schema/model.json", "build/generated/Model.eidos"],
    ["schema/model.json"],
    ["build/generated/Model.eidos"],
    []
);

Generated :: comptime build.generated_source(
    Emit,
    "build/generated/Model.eidos",
    "generate",
    "main"
);

BuildGraph :: comptime build.graph(Emit, [Generate], [Generated]);
```

- `build.read_text(Fs, path)` 只能读取 `fileInputs` 展开的文件；`build.environment(Env, name)` 只能读取已登记且存在的变量；
- `build.command` 只描述 host 上执行的登记工具、参数、输入、输出和 step dependency，不会在表达式求值时产生隐式副作用；
- 编译器验证 cycle、重复 output、未声明 input、缺失 dependency edge、output root、producer 与 target；随后仅携带声明的环境变量按稳定拓扑顺序执行，并确认每个声明 output 已实际生成；
- `generated_source` 的目录会自动加入当前 target 的 import roots。

## 验证与产物

```powershell
eidosc build --project . --target-name main --trace-build
eidosc build --project . --emit-build-graph build/graph.json
```

- `--trace-build` 输出 capability dependency、实际 capability access、graph hash、step/cache 状态和工具输出；
- `--emit-build-graph` 写出 canonical JSON；
- 相同 program、声明输入、环境、工具 hash、host/target 与 graph 会复用经过 output hash 验证的 BuildGraph cache；
- 成功构建提供确定性的 in-toto/SLSA provenance 与 CycloneDX 1.6 SBOM metadata；release profile 会拒绝 volatile provenance。

教程 CI 对 `examples/build_host` 执行 `build --target typed --no-cache --emit-build-graph` 并校验产物存在（见 `verify-examples.ps1`）。
