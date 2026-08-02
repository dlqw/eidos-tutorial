# 函数式编程

示例文件：`examples/functional/43_open_alias_trait_impl.eidos`、`examples/functional/44_std_traversable_alias_applicative.eidos`、`examples/functional/45_std_list_traversable_alias_applicative.eidos`、`examples/functional/46_traversable_alias_applicative_empty_cases.eidos`、`examples/functional/47_traversable_sequence_alias_applicative.eidos`、`examples/functional/48_sequence_result_applicative.eidos`、`examples/functional/49_generic_traversable_sequence.eidos`、`examples/functional/55_functional_infix_chain_style.eidos`

## 核心组合子

Prelude 提供 `Functor`、`Applicative`、`Monad`、`Foldable`、`Traversable`、`Semigroup`、`Monoid`、`Alternative` 等函数式契约（见基础 [集合类型](../basics/10-collections.md)）。函数式读法使用管道、组合与中缀：

```eidos
import std.Functions

(|+|) :: Int -> Int -> Int
{
    left => right => left + right + 10
}

main :: Unit -> Int
{
    _ => {
        infixed := 1 |+| 2;
        piped := 4 |> inc;
        composed := (inc >>> double)(3);
        infixed + piped + composed
    }
}
```

风格约定（2026-05-28 基线）：线性数据流优先写 `value |> f |> g`；函数组合优先写 `f >>> g` 或 `g <<< f`；`Functor` / `Applicative` / `Monad` 场景优先展示 `f <$> value`、`mf <*> mx`、`mx >>= f`；连续容器变换优先写链式调用，例如 `xs.map(f).filter(p).fold_left(seed)(step)`。当限定路径能显著降低歧义时，仍可保留 `Module.function(value)(arg)` 写法。普通分组调用 `function(value)` / `function(value, arg)` 也是稳定的默认调用风格，不会仅因为可写成链式或中缀而提示。CLI/IDE/LSP 会把可机械转换的连续柯里化前缀调用作为 help/hint 级风格建议给出 Quick Fix：`Seq.append(a)(b)` 可改为链式 `a.append(b)`，也可改为分组调用 `Seq.append(a, b)`。

## alias-backed 特化

用户自定义开放别名 / 深别名 `Applicative` / `Traversable` 实现会在多种路径上保持可用：

1. **开放别名反向匹配**：`Result.traverse(Ok(2))(produce_keep_edges)` 即使回调返回的是底层 `Triple[String, Int, Bool]`，也能反推出 `G = KeepEdges[String, Bool]`；`DeepBoxedResult[String]` 这类深别名链在 direct/helper 两条遍历路径上都能继续正确特化（`44_std_traversable_alias_applicative`）；
2. **递归 traversable**：`Seq.traverse([1, 2])(produce_keep_edges)` 在重复特化 `map2_applicative(cons)(...)` 的过程中持续保留用户自定义 open alias / deep alias `Applicative` impl（`45_std_list_traversable_alias_applicative`）；
3. **空/短路分支**：`Option.traverse(None())(...)` 与 `Seq.traverse([])(...)` 也会通过用户自定义 alias-backed `Applicative` 的 `pure` 正确完成特化，而不是只在回调被真正调用时才可达（`46_traversable_alias_applicative_empty_cases`）。

## `sequence` 与泛型入口

- `Option.sequence`、`Seq.sequence`、`Result.sequence` 是容器专用 helper，把 `Option[G[A]]`、`Seq[G[A]]`、`Result[G[A], E]` 稳定翻转成 `G[Option[A]]`、`G[Seq[A]]`、`G[Result[A, E]]`；alias-backed 覆盖与内建 `ResultWith[E]` 同构嵌套都能稳定通过（`47_traversable_sequence_alias_applicative`、`48_sequence_result_applicative`）；
- `std.Traversable.sequence` 是泛型外层容器版本，调用方不必先选定外层容器专用 helper；在 `Option`、`List`、`Result` 上同时穿过用户自定义 alias-backed applicative 和内建 `ResultWith[E]` 同构嵌套（`49_generic_traversable_sequence`）。

## 类型参数 trait 约束的位置

约束位置只接受 trait；effect 授权通过函数 `need` 从句表达（见基础 [泛型](../basics/08-generics.md) 与进阶 [效果系统](02-effects.md)）。
