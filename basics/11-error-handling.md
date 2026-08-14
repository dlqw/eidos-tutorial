# 错误处理

示例文件：`examples/basics/62_option_suffix_coalesce.eidos`、`examples/basics/63_let_question_option_result.eidos`

## `Option` 与 `Result`

Eidos 没有异常机制，错误通过返回值表达：

- `Option[A]`：`Some(value)` / `None()`，表达"可能没有值"；
- `Result[A, E]`：`Ok(value)` / `Err(error)`，表达"可能失败并携带错误"；canonical 写法为 `Result.With[E, A]`（`import std.Result` 后可用 `Result.With[String, Int]`）；
- `Either[L, R]`：`Left` / `Right`，right-biased，用于更一般的选择。

常用组合子（Prelude 提供）：`unwrap_or`、`map` / `fmap`、`and_then`、`or_else`、`flatten`、`apply`、`bind`、`traverse`、`fold_left` / `fold_right`、`is_ok` / `is_err` / `is_some`。详见 [集合类型](10-collections.md) 与进阶 [函数式编程](../advanced/05-functional-programming.md)。

## `Option` 后缀与 `??` 合并

参数类型写 `Int?` 表示"可能为 `Option[Int]`"，`??` 提供缺省值合并：

```eidos
import std.Option

fallback :: Int? -> Int {
    value => value ?? 42
}

from_some :: fallback(Some(7));
from_none :: fallback(None());
```

## `let?` 绑定

块内 `let? <pattern> = <expr>;` 展开 `Option` / `Result` 成功值，并在失败分支提前返回当前函数或 lambda：

```eidos
option_pipeline :: Int -> Option[Int]
{
    value => {
        let? first = maybe_positive(value);
        let? second = maybe_positive(first);
        Some(second + 1)
    }
}

result_pipeline :: Int -> Result.ResultWith[String, Int]
{
    value => {
        let? first = parse_positive(value);
        let? second = parse_positive(first);
        Ok(second + 1)
    }
}
```

约束：

1. `let?` 不是顶层声明，只能出现在有返回上下文的函数或 lambda 块内；
2. 右侧必须是 `Option[A]` 或 `Result[A, E]`；
3. `Option[A]` 只能用于返回 `Option[R]` 的上下文，失败分支返回 `None()`；
4. `Result[A, E]` 只能用于返回 `Result[R, E]` 或 canonical 等价 alias 的上下文，失败分支返回原 `Err(e)`；
5. 当前不做 `Option -> Result` 转换，也不做错误类型映射；
6. 左侧模式当前要求不可反驳，可反驳解构仍使用 `match` / `if let`。

降低规则：`let?` 只存在于 Parser/AST/NameResolver/Types 阶段；HIR 构建时会消除为普通 `match` + `return`。HIR/MIR/LLVM 中没有 `let?` 专用节点。

## 错误码体系

编译器诊断按错误码域组织（`E1xxx` 借用与能力、`E3xxx` 声明与命名、`E4xxx` 类型、`E5xxx` MIR、`W4xxx` 覆盖告警）。按错误域排查的完整指南见 [攻克编译错误](../errors/01-error-code-overview.md)。
