# 安装与验证环境

## 构建编译器

先确认编译器可运行（需要 .NET 8+ 与仓库内的 Eidosc 源码）：

```powershell
dotnet build src/Eidosc/Eidosc.sln
dotnet run --project src/Eidosc/Eidosc.Cli -- --help
```

`eidosc` CLI 常用入口：

```powershell
# 语法/语义分析示例文件到指定阶段（parser / hir / mir / llvm / types）
dotnet run --project src/Eidosc/Eidosc.Cli -- analyze examples/basics/02_functions_calls.eidos --phase hir --deny style

# 调试输出
dotnet run --project src/Eidosc/Eidosc.Cli -- debug projects/test/src/basic/literals.eidos --debug-output projects/test/debug --debug-level diagnostic

# IDE/LSP 单文件模式
dotnet run --project src/Eidosc/Eidosc.Cli -- ide projects/test/src/basic/literals.eidos --stdin --phase types
```

## 运行教程验证脚本

教程所有可运行示例都通过 `verify-examples.ps1` 自动校验（递归扫描 `examples/`，`examples/build_host/` 作为独立 BuildGraph 项目单独验证）：

```powershell
powershell -ExecutionPolicy Bypass -File docs/tutorial/verify-examples.ps1
```

脚本要求：

- 能找到 `Eidosc.Cli` 项目（默认在 `Eidosc/src/Eidosc.Cli`，可用 `-CliProject` 指定）；
- 每个示例默认分析到 `hir` 阶段，`--deny style` 把风格告警视为失败；
- 个别示例有特殊预期（`06_nested_call_parser_only` 只要求到 parser 阶段；`36_hkt_trait_constraint_kind_mismatch` 预期失败并校验错误文本），保持文件名即可被脚本识别。

## 编辑器支持

Eidos 提供 VS Code、Neovim 与 JetBrains 三套编辑器插件（位于本仓库 workspace 的 `tools/editor/`），支持 diagnostics、completion、go to definition、find references、hover 与 LSP semantic output。安装后可用 `.eidos` 文件直接体验教程示例。

## 排查类型推断

需要排查类型推断时，建议使用 `debug --debug-level diagnostic` 查看 `substitution` 输出；当前已包含类型变量 `raw/resolved`、绑定链（chain）与 AST 上下文位置。
