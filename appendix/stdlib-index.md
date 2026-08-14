# 附录 D：Std 导出面索引

标准库的权威索引是编译器实时导出的导出面：

```powershell
dotnet run --project Eidosc/src/Eidosc.Cli -- info --stdlib
```

## 能力分组速查

| 能力 | 显式模块 | 代表接口 |
| --- | --- | --- |
| 数学与几何 | `std.Math`、`std.FloatMath`、`std.GameMath` | 重载的 `Math.abs`、`GameMath.add`、`GameMath.scale` |
| 控制台 IO | `std.Console` | 泛型 `Console.write`、`Console.write_line`、`Console.read_line`、`Console.write_char_code` |
| 文本与文件 | `std.Text`、`std.File` | `Text.trim`、`File.read_text`、`File.write_text` |
| 容器 | `std.SeqBuilder`、map、set、queue、stack | builder 与专用容器操作 |
| 网络与序列化 | `std.Network`、`std.Binary`、`std.Json`、`std.JsonParser`、`std.JsonValue` | HTTP、二进制 codec、JSON 构造与解析 |

Prelude Core（自动 open，非 package）与 `std` 的分工见 [标准库使用](../practices/02-standard-library.md) 与 [集合类型](../basics/10-collections.md)。

## 语法变更史

教程版本与 Eidos 语言版本一致（当前 `0.8.0-alpha.1`，见 [`eidos-language.toml`](../eidos-language.toml)）。按版本整理的变更记录在 [`changelogs/`](../changelogs/)：

- `changelogs/<exact-semver>.md`：已发布版本的 release notes；
- `changelogs/<target-semver>/`：开发中的 fragment（文件名包含目标版本）。

语法/语义变化必须同步教程正文、examples、fixture 与三个编辑器插件（见 [示例验证与测试](../toolchain/02-verification-and-testing.md)）。
