# 高阶类型与 kind

示例文件：`examples/generics/31_hkt_parenthesized_kind.eidos`、`examples/generics/32_hkt_adt_inferred_kind.eidos`、`examples/generics/33_hkt_effect_polymorphism.eidos`、`examples/generics/34_hkt_trait_inferred_kind.eidos`、`examples/generics/35_hkt_trait_constraint_type_args.eidos`、`examples/generics/36_hkt_trait_constraint_kind_mismatch.eidos`、`examples/generics/37_trait_impl_generic_trait_args.eidos`

kind 基础（`kind1` / `kind2` / `kind3`、推断、累积统一、where 从句）见基础 [泛型](../basics/08-generics.md)。本章讲高阶类型在 trait 与 ADT 中的完整应用。

## 高阶 trait 参数

trait 声明支持一等 type params，方法签名可以引用高阶参数：

```eidos
HK[K] :: trait { run :: K[Box] -> Self }
```

未显式标注时，trait 类型参数会根据方法签名自动推断 kind（`K[Box]` 推断 `K: kind2 -> kind1`），并在 symbol/HIR 层保留元数据。

## 部分应用

具名类型构造器在高阶位置支持部分应用。例如 `Lift[F: kind2]` 下可写 `Lift[Either[String]]`（`Either[String]` 仍是等待一个参数的构造器）。

## kind 兼容检查

- kind 兼容检查同时覆盖"类型参数构造器应用"（如 `F[G]`）与"ADT 类型实参"（如 `Lift[F: kind2]` 下的 `Lift[Int]` 会报错）；
- 类型参数 trait 约束支持模块限定与类型实参（如 `T: Core.Functor[Box]`）；Types 阶段会校验 trait 实参数量与 kind；
- 冲突约束报错示例见 `examples/generics/36_hkt_trait_constraint_kind_mismatch.eidos`（预期失败，错误文本含 `kind`）。

## 开放别名反向匹配

作为高阶 trait 实参使用的开放别名，会参与类型推断与 MIR 特化时的反向匹配。例如 `ApplicativeKeepEdgesStringBool :: instance Applicative[KeepEdges[String, Bool]]` 可以在外层期望 `Triple[String, A, Bool]` 时满足 `G[A]`（`KeepEdges[String, Bool] = Triple[String, _, Bool]`）：

```eidos
lift[A, G: kind2 : Applicative[G]] :: A -> G[A]
```

调用 `lift` 时若返回类型是 `Triple[String, Int, Bool]`，编译器能从开放别名反推出 `G = KeepEdges[String, Bool]`。完整示例见 `examples/generics/37_trait_impl_generic_trait_args.eidos` 与 `examples/functional/43_open_alias_trait_impl.eidos`。

## 效果多态与 kind

`E: effects` 是 effect 行参数（不是类型 kind）；值类型的高阶 kind 使用 `kind1` / `kind2` / `kind3`。同时使用类型构造器与 effect 行的示例见 `examples/generics/33_hkt_effect_polymorphism.eidos`（配合进阶 [效果系统](02-effects.md)）。
