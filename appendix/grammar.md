# 附录 A：语法参考（BNF 摘要）

核心 BNF 摘要见 [`BNF.zh-CN.md`](../BNF.zh-CN.md)（英文版 [`BNF.en.md`](../BNF.en.md)）。

语法权威来源为 `src/Eidosc/Eidosc/Grammar/GrammarDefine.cs`，验证基线为 2026-03-19 的本地执行结果。

## 常用语法形态速查（0.8 基线）

| 形态 | 写法 | 示例 |
| --- | --- | --- |
| 顶层常量绑定 | `name :: expr;` | `answer :: 42;` |
| 局部绑定 | `name := expr;` | `y := x + 1;` |
| 可重赋值绑定 | `mut name := expr;` | `mut counter := 1;` |
| 解构绑定 | `(pattern) := expr;` | `(a, b) := pair;` |
| 引用绑定 | `ref x := expr;` / `mref x := expr;` | `ref ra := a;` |
| 函数声明 | `name :: Sig { body }` | `inc :: Int -> Int { x => x + 1 }` |
| 柯里化 binder list | `p1, p2 => expr` | `left, right => left + right` |
| 类型声明 | `Name :: type { ... }` | `Option[T] :: type { Some:: type(T), None :: type {} }` |
| 类型别名 | `Name :: type = Type;` | `UserId :: type = Int;` |
| trait 声明 | `Name :: trait { ... }` | `Show :: trait { show :: Self -> String }` |
| instance | `Name :: instance Trait { ... }` | `ShowPoint :: instance Show { ... }` |
| effect 声明 | `Name :: effect;` | `Logger :: effect;` |
| effect 授权 | `need E1, E2` | `main :: Unit -> Int need io` |
| 模块声明 | `Path.Name :: module { ... }` | `Demo.Base :: module { ... }` |
| import | `import Module.Path` / `import Module.Path.{A, B}` | `import std.Option` |
| 匹配 | `match e { p => expr, ... }` | 见 [模式匹配](../basics/07-pattern-matching.md) |
| 守卫 | `pattern when guard => expr` | `n when n > 0 => 1` |
| 决策表 | `decide fallback { pred(_): keys => value }` | 见 [模式匹配](../basics/07-pattern-matching.md) |
| 属性 | `@[name(...)]` | `@[expand(derive_marker)]`、`@[derive(Eq)]` |
| extern | `@[extern(c, name: "...")]` | 见 [FFI](../advanced/08-ffi.md) |
| comptime 绑定 | `NAME :: comptime expr;` | `USER_INFO :: comptime meta.shape_of(User);` |

## 已移除 / 迁移形态

- `import Module.Path as Alias`（keyword 形式）只在 `legacy` 语法模式中接受；新写法 `Alias :: import Module.Path;`
- `@impl(Trait)` 函数级 attribute 只作为迁移输入识别
- `view(...)` / `View(...)` 模式移除，使用 `(expr -> pattern)`
- `noImplicitStdlib` 移除，使用 `noImplicitPrelude`
- 旧 `&expr` 仍兼容但降为过渡写法；正式表面为 `ref` / `mref`
