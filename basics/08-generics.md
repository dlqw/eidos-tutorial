# 泛型

示例文件：`examples/generics/31_hkt_parenthesized_kind.eidos`、`examples/generics/32_hkt_adt_inferred_kind.eidos`、`examples/generics/33_hkt_effect_polymorphism.eidos`、`examples/generics/34_hkt_trait_inferred_kind.eidos`、`examples/generics/35_hkt_trait_constraint_type_args.eidos`、`examples/generics/36_hkt_trait_constraint_kind_mismatch.eidos`、`examples/traits/50_qualified_trait_method_paths.eidos`

## 泛型参数

函数、ADT、type alias、trait 和 named instance 引用都使用同一套有序 generic argument 规则：

```eidos
identity[T] :: T -> T
{
    x => x
}
```

## kind：类型的类型

泛型参数可以带 kind 注解，描述"它接受几个类型参数"：

1. 一元构造器 kind：`F: kind2`；
2. 多参数构造器 kind：`F: kind3`；
3. 支持带括号的高阶 kind：`F: kind2 -> kind1`；
4. kind 兼容检查同时覆盖"类型参数构造器应用"（如 `F[G]`）与"ADT 类型实参"（如 `Lift[F: kind2]` 下的 `Lift[Int]` 会报错）；
5. 具名类型构造器在高阶位置支持部分应用（例如 `Lift[F: kind2]` 下可写 `Lift[Either[String]]`）；
6. 未显式标注 kind 时会根据类型用法自动推断（例如 `F[A]` 会推断 `F: kind2`）；
7. 未显式标注的 ADT 类型参数 kind 也会根据构造器/别名中的类型使用自动推断（例如 `type Lift[F] { Lift(F[Int]) }` 推断 `F: kind2`，`type UseK[K] { UseK(K[Box]) }` 推断 `K: kind2 -> kind1`）；
8. Effect 行参数必须显式使用 `effects` kind（例如 `E: effects`）；值类型的高阶 kind 继续使用 `kind1`、`kind2` 或 `kind3`；
9. kind 约束采用累积统一（unification）：同一类型变量的多条 kind 约束共享同一推断状态，不兼容约束（如同时要求 `kind2` 与 `kind1`）会被稳定报错；
10. trait 声明支持一等 type params 语法（如 `Functor[F: kind2] :: trait { ... }`），未显式标注时根据方法签名自动推断 kind（例如 `HK[K] :: trait { run :: K[Box] -> Self }` 推断 `K: kind2 -> kind1`）；
11. 类型参数 trait 约束支持模块限定与类型实参（如 `T: Core.Functor[Box]`）；Types 阶段会校验 trait 实参数量与 kind；
12. 类型参数 trait 约束位置只接受 trait；effect 授权通过函数 `need` 从句表达（如 `String -> Unit need Writer`）；
13. 泛型约束支持轻量 `where` 从句，可把复杂 kind/trait 约束移出参数列表：

```eidos
lift[A, G] :: A -> G[A] where G: kind2, G: Applicative[G]
```

## 多约束参数列表

类型参数列表内同时出现 kind 与 trait 约束（`kind2 : Applicative.Applicative[G]` 中的 `:` 分隔 kind 与 trait 约束）：

```eidos
lift_alias[A, G: kind2 : Applicative.Applicative[G]] :: A -> G[A]
{
    value => Applicative.pure(value)
}
```

## 值域泛型

`comptime N: Int` 声明值域泛型参数；`comptime T: Type` 仍声明类型域参数。参数顺序就是应用顺序，编译器在 AST、symbol、HIR/MIR、缓存和 IDE 中保留 type/value/effect-row 三种 domain。完整规则见进阶 [值域泛型与 const generics](../advanced/04-const-generics.md)。

## 泛型函数的 trait bound

泛型函数的 trait bound 会在调用点实例化后强制检查（例如 `id[T: Marker] :: T -> T` 在 `Int` 未实现 `Marker` 时调用 `id(1)` 会报错）。
