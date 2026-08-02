# 变量绑定与解构

示例文件：`examples/basics/12_let_pattern.eidos`、`examples/pattern/15_pattern_binding_modes.eidos`

## 解构绑定

块级解构使用模式左侧绑定，0.8 基线写法为 `(pattern) := expr;`：

```eidos
sum_pair :: (Int, Int) -> Int
{
    pair => {
        (a, b) := pair;
        a + b
    }
}
```

`(a, b) := pair` 把元组解构为两个局部值，在 NameResolver/Types/HIR/MIR 贯通。块级 `let <pattern> = <expr>;` 形式（README 基线文档化）同样可用，**当前要求"不可反驳模式"**（irrefutable pattern）；可反驳场景请使用 `match`（见 [模式匹配](07-pattern-matching.md)）。

## 引用绑定 mode

模式绑定支持显式引用 mode，0.8 写法为 `ref x := ...` / `mref x := ...`：

```eidos
binding_modes :: (Int, Int) -> Ref[Int]
{
    pair => {
        (a, b) := pair;
        ref ra := a;
        mref mb := b;
        match a
        {
            (a as ref keep) => keep
        }
    }
}
```

规则：

1. `ref x := ...` 产生 `Ref[T]` 绑定；`mref x := ...` 产生 `MRef[T]` 绑定；
2. 模式内 `p as ref x` / `p as mref x` 会保留绑定 mode；
3. 可重赋值绑定使用 `mut x := ...`；只有 `mut`（或 `let mut`）绑定可以作为 `:=` 赋值目标；
4. `or-pattern` 中同名绑定必须使用一致的绑定 mode，否则会报 `E3000`。

`ref` / `mref` 是 Eidos 显式引用语义的第一阶段表面：`ref expr` 在 Types 阶段返回 `Ref[T]`，`mref expr` 返回 `MRef[T]`，`*expr` 要求操作数是 `Ref[T]` 或 `MRef[T]`。完整规则见进阶 [所有权与借用](../advanced/01-ownership-and-borrowing.md)。

## 绑定与赋值

0.8 基线中局部不可变绑定使用 `name := expr;`，可重赋值绑定使用 `mut name := expr;`；`name :: expr;` 是顶层声明。命名分层与字面量见 [第一个程序](../getting_started/03-first-program.md)。
