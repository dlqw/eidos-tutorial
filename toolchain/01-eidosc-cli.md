# eidosc CLI

`eidosc`（`dotnet run --project Eidosc/src/Eidosc.Cli --`）是 Eidos 编译器的命令行入口。

## 常用命令

| 命令 | 用途 |
| --- | --- |
| `analyze <file> --phase <phase> --deny style --no-color` | 分析单个文件到指定阶段；`--deny style` 把风格告警视为失败 |
| `debug <file> --debug-output <dir> --debug-level diagnostic` | 调试输出（`substitution`、`*_borrow_aliases`、`*_loan_constraint_states` 等） |
| `ide <file> --stdin --phase types` | IDE/LSP 单文件模式 |
| `meta expand <file> --format json / --emit-generated <dir> / --trace-comptime --comptime-budget N` | 编译期元编程展开 |
| `build --project . --target-name main --trace-build / --emit-build-graph <file> / --no-cache` | Build host 构建 |
| `info --stdlib` | 显式标准库实时导出面 |

## 分析阶段（phase）

示例文件可分析到以下阶段：`parser`、`hir`、`mir`、`llvm`、`types`。默认教程验证阶段为 `hir`；个别文件有特殊预期（见 [示例验证与测试](02-verification-and-testing.md)）。

```powershell
dotnet run --project src/Eidosc/Eidosc.Cli -- analyze examples/basics/02_functions_calls.eidos --phase hir --deny style
```

## 调试输出

需要排查类型推断时，建议使用 `debug --debug-level diagnostic` 查看 `substitution` 输出；当前已包含类型变量 `raw/resolved`、绑定链（chain）与 AST 上下文位置。借用诊断的 alias trace 反查见 [错误码体系总览](../errors/01-error-code-overview.md)。

## manifest 与项目配置

`eidos.toml` 的完整说明见 [Hello World 与项目配置](../getting_started/02-hello-world.md)；`[build]` 段见 [Build host 与 BuildGraph](../advanced/07-build-host.md)。
