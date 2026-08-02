# Hello World 与项目配置

## 最小项目清单

Eidos 项目使用 `eidos.toml` 描述语言版本与包信息。最小配置如下：

```toml
manifestSchema = 3

[language]
version = "0.8.0-alpha.1"

[package]
name = "dev.eidos.app"
version = "0.1.0"
```

其中：

1. `language.version` 是用户应主动设置的 Eidos 语言 SemVer；
2. `manifestSchema` 是 manifest schema 版本，当前纳入版本控制的 manifest 应显式声明 `3`；
3. `[package].name` 使用点号 Namespace 形式的包名。

声明默认私有，只有 `export` 才形成跨模块 API，`compiler(internal)` 永不泄漏。`[language].noImplicitPrelude = true` 可关闭自动 core-image open；已移除的 `noImplicitStdlib` 会直接报错。

## 需要标准库能力时

非核心能力属于显式 `std` package，在 `[dependencies]` 中声明：

```toml
[dependencies]
std = "0.1.0-alpha.1"
```

## 第一个程序

```eidos
main :: Unit -> Int need io {
    print("hello");
    println();
    0
}
```

要点：

- `main :: Unit -> Int`：函数声明，运行时参数规范化为 `Unit` 时，普通 block 隐式等价于唯一的 `_ => block` 分支（详见 [语句与表达式](../basics/03-statements-and-expressions.md)）；
- `need io`：声明函数需要的 effect 授权（详见 [效果系统](../advanced/02-effects.md)）；
- `print` / `println` 是普通的 `Display` trait 约束重载，并非编译器特判（详见 [格式化输出](../basics/13-formatting-output.md)）；
- 函数的返回值是块内最后一个表达式（尾表达式），`0` 即返回值。

## 验证

`verify-examples.ps1` 之外，教程示例都可以用 CLI 直接验证：

```powershell
dotnet run --project src/Eidosc/Eidosc.Cli -- analyze <file>.eidos --phase hir --deny style
```

构建级验证（Build host 项目）使用：

```powershell
eidosc build --project . --target-name main --trace-build
```
