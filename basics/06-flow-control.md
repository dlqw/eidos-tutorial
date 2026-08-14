# 流程控制

示例文件：`examples/basics/28_early_return.eidos`、`examples/basics/71_implicit_unit_selection.eidos`、`examples/pattern/13_if_let_pattern.eidos`、`examples/pattern/14_while_let_pattern.eidos`

## `if` 分支

`if` 是表达式，`then` / `else` 两个分支都求值后选择其一：

```eidos
normalize_nonneg :: Int -> Int
{
    x => if x < 0 then { return 0 } else { x }
}
```

`if`、`if let`、`while let` 和完整 `match` 均保留为控制流入口（0.8 基线）。

## `then` / `else` 选择器（0.8）

`then` / `else` 对 `Bool`、`Option`、`Result` 和 right-biased `Either` 提供固定二分支选择。`_0`、`_1` 是当前 arm 的 payload 占位符；单臂缺失侧为 `Unit`：

```eidos
render_result :: Result[Int, String] -> Int
{
    result => result
        then _0
        else 0
}
```

多个 subject 写成括号 tuple，全部从左到右求值一次，group `else` 不提供占位符：

```eidos
main :: Unit -> Int
{
    maybe: Option[Int] := Some(20);
    parsed: Result[Int, String] := Ok(21);
    choice: Either[String, Int] := Right(1);
    (maybe, parsed, choice)
        then _0 + _1 + _2
        else 0
}
```

formatter 不为单表达式 arm 添加花括号；只有含绑定、赋值或多条语句的 arm 使用 block。

## `if let` 模式分支

`if let <pattern> = <expr> then <expr> else <expr>` 语义上会降低为 `match`：

```text
if p := e then { t } else { f }
==> match e { p => { t }, _ => { f } }
```

短分支也可以直接写成 `if p := e then t else f`。

绑定作用域规则：

1. `pattern` 中的绑定变量仅在 `then` 分支可见；
2. `else` 分支与后续外层作用域不会看到这些绑定；
3. 没有 `else` 时，默认回退分支值为 `Unit`。

## `while let` 模式循环

`while let <pattern> = <expr> then <block>` 按循环匹配降低：

```text
while p := e then { body }
==> loop { match e { p => { body; () }, _ => break } }
```

当前行为：

1. `pattern` 绑定仅在循环体块中可见；
2. 匹配失败时自动退出循环（内部使用 `break` 语义）；
3. 表达式类型固定为 `Unit`，用于流程控制而不是值计算；
4. 块内在 `if` / `if let` / `while let` 语句后仍可继续写无分号尾表达式作为块值。

## 早返回

`return <expr>` 提前结束当前函数（见 [语句与表达式](03-statements-and-expressions.md)）。
