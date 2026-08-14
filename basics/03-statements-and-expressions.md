# 语句与表达式

示例文件：`examples/basics/07_block_result_known_issue.eidos`、`examples/basics/28_early_return.eidos`

Eidos 是表达式语言：块（block）是表达式，其值是块内最后一个表达式（尾表达式）的结果。

## 块表达式与尾表达式

块内语句依次执行，块的值由尾表达式决定：

```eidos
add_1_via_block :: Int -> Int
{
    x => {
        y := x + 1;
        y          // 尾表达式，即块值
    }
}
```

块内在 `if` / `if let` / `while let` 语句后仍可继续写无分号尾表达式作为块值（例如 `...; total`）。

## 隐式 `Unit` body

首个运行时参数规范化为 `Unit` 时，普通 block 隐式等价于唯一的 `_ => block` 分支；显式写法仍然合法：

```eidos
main :: Unit -> Int
{
    initialize();
    run()
}
```

## 早返回 `return`

`return <expr>` 会把 `<expr>` 与当前函数/匿名函数声明返回类型做统一（不再退化成 `Unit`），可消除合法早返回路径上的 `Int` vs `()` 假冲突。早返回的类型与 lowering 链路已贯通到 Types/HIR/MIR/LLVM：返回值按函数返回类型校验，HIR 保留专用 return 节点，MIR/LLVM 可稳定生成返回终止指令。

```eidos
normalize_nonneg :: Int -> Int
{
    x => if x < 0 then { return 0 } else { x }
}

add_one_nonneg :: Int -> Int
{
    x => {
        n := normalize_nonneg(x);
        n + 1
    }
}
```

## 嵌套调用与块值

嵌套调用在柯里化签名下可通过 Parser + Types，例如 `add(1)(2)`（见 [函数](04-functions.md)）。块表达式的尾表达式可正确作为块值参与返回类型统一。
