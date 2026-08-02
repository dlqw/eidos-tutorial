# 集合类型

示例文件：`examples/basics/08_list_comprehension.eidos`、`examples/stdlib/29_precompiled_stdlib.eidos`、`examples/stdlib/42_stdlib_safe_and_traits.eidos`

## Prelude Core Image

Eidosc 把预编译的 **Prelude Core Image** 与普通 `std` package 严格分开。Prelude 不是 package，会自动 open，承载语言 elaboration 所需的核心函数式契约和类型：`Display`、`Option`、`Result`、`Either`、`Ordering`、`Seq`、`Functor`、`Applicative`、`Monad`、`Foldable`、`Traversable`、`Semigroup`、`Monoid`、`Alternative`。

`print` / `println` 是普通的 `Display` trait 约束重载，并非编译器特判；基础类型和核心 ADT 都提供 `Display` instance。operator 与 `do` elaboration 按 Prelude 声明注册的 compiler-owned semantic role 查找符号，不再依赖硬编码 `std.Module.function` 路径。

`[language].noImplicitPrelude = true` 可关闭自动 core-image open（见 [Hello World](../getting_started/02-hello-world.md)）。

## `Seq` 与列表字面量

`Seq[Int]` 用列表字面量构造：`[1, 2, 3]`、`[x, x]`、`[]`。常用操作（`import std.Seq` 后可直接调用，也可用限定路径）：

| 操作 | 说明 |
| --- | --- |
| `head(xs)` / `tail(xs)` | 首元素 / 尾部（`Option` 返回） |
| `len(xs)` | 长度 |
| `map(xs)(f)`、`filter(xs)(p)` | 变换 / 过滤 |
| `fold_left(xs)(acc)(step)`、`fold_right(xs)(acc)(step)` | 折叠 |
| `zip(xs)(ys)`、`zip_with(xs)(ys)(f)` | 拉链 |
| `find(xs)(p)` | 查找（`Option` 返回） |
| `Seq.apply(fs)(xs)`、`bind(xs)(f)` | Applicative / Monad 组合 |

```eidos
via_seq :: fold_left(filter(map([1, 2, 3])(inc))(gt_two))(0)(sum_left);
via_head := unwrap_or(head([1, 2, 3]))(0);
via_tail := match tail([1, 2, 3])
{
    Some(rest) => len(ref rest),
    None() => 0
};
```

## 列表推导式

列表推导式是序列构造的声明式写法，当前支持子集：

```eidos
doubled :: [x * 2 | x <- [1, 2, 3]];
```

当前状态：

1. AST/NameResolver/Types 已支持多限定符（多 generator + guard）的作用域与类型检查；
2. HIR 已保留专用 `ListComprehension` 结构（不再在 HIR 早期展开为常量列表）；
3. MIR 已支持 CFG lowering（循环 + guard），并通过 runtime `array_new` / `array_push` 构建结果列表；
4. MIR 已支持"长度运行时求值"的生成器循环（使用 runtime `array_length` 调用）；
5. LLVM lowering 会把索引读写映射为 runtime `array_get` / `array_set`；
6. CFG join/backedge 的局部值更新已修复为 slot 语义（entry `alloca` + `load/store`），避免循环回边读到初始 alias；
7. Native smoke 已支持 clang-only 链路：编译器会自动注入 `main -> eidos_main` 入口桥接，不再强依赖 `llc`；
8. 非 `VarPattern` 的生成器模式当前被明确禁止，并在 MIR 阶段报告 `error[E5101]`；生成器请使用简单变量或通配绑定。

## 容器选择理念

Eidos 标准库保持最少且最正交的用户抽象：你选择语义（`Option` / `Result` / `Seq` / `Either`），编译器选择实现（布局、增长策略、专用化与性能策略都下沉到编译器）。不要在同类容器之间做编译器本可自动完成的取舍。

## 其余 `std` 能力

非核心能力属于显式 `std` package（`[dependencies] std = "0.1.0-alpha.1"`）：`std.Math` / `std.FloatMath` / `std.GameMath`（数学与几何）、`std.Console`（控制台 IO）、`std.Text` / `std.File`（文本与文件）、容器（builder/map/set/queue/stack）、`std.Network` / `std.Binary` / `std.Json`（网络与序列化）。完整能力分组见 [开发实践与标准库](../practices/02-standard-library.md)。
